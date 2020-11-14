#include "mainwindow.h"

#include <QDesktopWidget>
#include <QFormLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QScrollBar>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QSize size = QDesktopWidget().availableGeometry(this).size();
  resize(size);

  tab = new QTabWidget;
  tab->addTab(initialize_tab_strategy(), tr("Strategy"));
  tab->addTab(initialize_tab_parameters(), tr("Parameters"));
  tab->addTab(initialize_tab_prevalence(), tr("Prevalence estimator"));
  tab->setCurrentIndex(0);

  chart = new QWidget;
  log = new QWidget;

  scroll_tab = new QScrollArea(this);
  scroll_tab->setWidget(tab);
  scroll_tab->setWidgetResizable(true);
  scroll_tab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

  QScrollArea *scroll_log = new QScrollArea(this);
  scroll_log->setWidget(log);
  scroll_log->setWidgetResizable(true);
  scroll_log->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

  QScrollArea *scroll_chart = new QScrollArea(this);
  scroll_chart->setWidget(chart);
  scroll_chart->setWidgetResizable(true);
  scroll_chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(scroll_tab, 0, 0, 1, 1);
  mainLayout->addWidget(scroll_log, 0, 1, 1, 1);
  mainLayout->addWidget(scroll_chart, 1, 0, 2, 2);

  QWidget *centralWidget = new QWidget;
  centralWidget->setLayout(mainLayout);

  this->setCentralWidget(centralWidget);
}

QDoubleSpinBox *MainWindow::create_parameter_DoubleSpinBox(QWidget *parent,
                                                           double min,
                                                           double max, int dec,
                                                           double val) {
  QDoubleSpinBox *box = new QDoubleSpinBox(parent);
  box->setMinimum(min);
  box->setMaximum(max);
  box->setDecimals(dec);
  box->setValue(val);
  box->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  return box;
}

QSpinBox *MainWindow::create_parameter_SpinBox(QWidget *parent, int min,
                                               int max, int val) {
  QSpinBox *box = new QSpinBox(parent);
  box->setMinimum(min);
  box->setMaximum(max);
  box->setValue(val);
  box->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  return box;
}

QWidget *MainWindow::initialize_tab_strategy() {
  QLabel *label_mode = new QLabel(tr("Simulation mode:"));
  label_mode->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  this->mode_ComboBox = new QComboBox(this);
  this->mode_ComboBox->addItems(
      QStringList{"contact management", "isolation", "incoming travelers"});
  this->mode_ComboBox->setCurrentIndex(0);

  QLabel *label_time_passed =
      new QLabel((std::string{"Time passed since "} +
                  this->mode_map_int[this->mode_ComboBox->currentIndex()] +
                  std::string{" [days]:"})
                     .c_str());
  label_time_passed->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  this->time_passed = create_parameter_DoubleSpinBox(
      this, 0, 21, 0, this->default_values["time_passed"]);

  QLabel *label_quarantine = new QLabel(tr("Duration of quarantine [days]:"));
  label_quarantine->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  this->quarantine = create_parameter_DoubleSpinBox(
      this, 0, 35, 0, this->default_values["quarantine"]);
  this->pre_risk = create_parameter_DoubleSpinBox(this, 0, 1, 3, 1);

  this->use_symptomatic_screening = new QCheckBox;
  use_symptomatic_screening->setChecked(true);
  use_symptomatic_screening->setSizePolicy(QSizePolicy::Maximum,
                                           QSizePolicy::Maximum);

  this->use_symptomatic_screening = new QCheckBox;
  use_symptomatic_screening->setChecked(true);
  use_symptomatic_screening->setSizePolicy(QSizePolicy::Maximum,
                                           QSizePolicy::Maximum);

  this->test_days_box = new QGroupBox(this);
  this->test_days_box->setTitle(tr("Days to test on:"));

  this->adherence = create_parameter_DoubleSpinBox(this, 0, 100, 0, 100);

  this->run_PushButton = new QPushButton(this);
  run_PushButton->setText(tr("Run"));
  run_PushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  QLabel *test_type_label = new QLabel(tr("Type of test: "));
  this->test_type = new QComboBox(this);
  this->test_type->addItems(QStringList{"PCR", "RDT"});
  this->test_type->setCurrentIndex(0);

  QHBoxLayout *test_type_layout = new QHBoxLayout;
  test_type_layout->addWidget(test_type_label);
  test_type_layout->addWidget(this->test_type);
  test_type_layout->addStretch();
  test_type_layout->setSizeConstraint(QLayout::SetFixedSize);

  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(label_mode, 0, 0);
  gridLayout->addWidget(mode_ComboBox, 0, 1, Qt::AlignLeft);
  gridLayout->addWidget(new QLabel(tr("Probability (pre-)infectious: ")), 1, 0);
  gridLayout->addWidget(pre_risk, 1, 1, Qt::AlignLeft);
  gridLayout->addWidget(label_time_passed, 2, 0);
  gridLayout->addWidget(time_passed, 2, 1, Qt::AlignLeft);
  gridLayout->addWidget(label_quarantine, 3, 0);
  gridLayout->addWidget(quarantine, 3, 1, Qt::AlignLeft);
  gridLayout->addWidget(new QLabel(tr("Symptomatic screening")), 4, 0);
  gridLayout->addWidget(use_symptomatic_screening, 4, 1, Qt::AlignLeft);
  gridLayout->addWidget(new QLabel(tr("Expected adherence [%]:")), 5, 0);
  gridLayout->addWidget(adherence, 5, 1, Qt::AlignLeft);
  gridLayout->setHorizontalSpacing(10);
  gridLayout->setSizeConstraint(QLayout::SetFixedSize);

  QVBoxLayout *strategy_tab_layout = new QVBoxLayout;
  strategy_tab_layout->addItem(gridLayout);
  strategy_tab_layout->addWidget(test_days_box);
  strategy_tab_layout->addItem(test_type_layout);
  strategy_tab_layout->addWidget(run_PushButton);
  strategy_tab_layout->setSizeConstraint(QLayout::SetFixedSize);
  strategy_tab_layout->setAlignment(Qt::AlignTop);

  connect(mode_ComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(mode_ComboBox_currentIndexChanged(int)));
  connect(time_passed, SIGNAL(valueChanged(double)), this,
          SLOT(time_passed_valueChanged()));
  connect(quarantine, SIGNAL(valueChanged(double)), this,
          SLOT(quarantine_valueChanged()));
  connect(run_PushButton, SIGNAL(clicked()), this,
          SLOT(run_PushButton_clicked()));

  initialize_test_date_checkboxes();

  QPixmap pic(":/logo.jpg");
  QLabel *logo = new QLabel(this);
  logo->setPixmap(pic);
  gridLayout->addWidget(logo, 0, 2, 3, 1);

  QWidget *widget = new QWidget;
  widget->setLayout(strategy_tab_layout);
  return widget;
}

QWidget *MainWindow::initialize_tab_parameters() {
  // variables
  QLabel *label_tau_inc =
      new QLabel(tr("Mean duration of incubation period [day]:"));
  QLabel *label_percentage_predetect =
      new QLabel(tr("Percentage thereof pre-detectable [%]:"));
  QLabel *label_tau_symp =
      new QLabel(tr("Mean duration of symptomatic period [day]:"));
  QLabel *label_asymp =
      new QLabel(tr("Percentage of asymptomatic infections [%]:"));
  QLabel *label_tau_post = new QLabel(
      tr("Mean duration of post-symptomatic, detectable period [day]:"));

  QLabel *label_pcr_sens = new QLabel(tr("PCR-test sensitivity [%]"));
  QLabel *label_pcr_spec = new QLabel(tr("PCR-test specificity [%]"));

  QLabel *label_mean = new QLabel(tr("Typical"));
  QLabel *label_lev = new QLabel(tr("Lower extreme value"));
  QLabel *label_uev = new QLabel(tr("Upper extreme value"));

  this->inc_lev = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["inc_lev"]);
  this->inc_mean = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["inc_mean"]);
  this->inc_uev = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["inc_uev"]);

  this->percentage_predetection = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["percentage_predetection"]);

  this->symp_lev = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["symp_lev"]);
  this->symp_mean = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["symp_mean"]);
  this->symp_uev = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["symp_uev"]);

  // this->post_lev = create_parameter_DoubleSpinBox(this, 0.01, 100, 2,
  // this->default_values["post_lev"]);
  this->post_mean = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["post_mean"]);
  // this->post_uev = create_parameter_DoubleSpinBox(this, 0.01, 100, 2,
  // this->default_values["post_uev"]);

  this->percentage_asymptomatic = create_parameter_DoubleSpinBox(
      this, 0.00, 100, 2, this->default_values["percentage_asymptomatic"]);

  this->pcr_sens = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["pcr_sens"]);
  this->pcr_spec = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["pcr_spec"]);

  this->rel_antigen_sens = create_parameter_DoubleSpinBox(
      this, 0.01, 100, 2, this->default_values["relative_antigen_sens"]);

  reset_PushButton = new QPushButton(this);
  reset_PushButton->setText(tr("Reset defaults"));
  reset_PushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  connect(reset_PushButton, SIGNAL(clicked()), this,
          SLOT(reset_PushButton_clicked()));

  // layout
  QGridLayout *param_tab_layout = new QGridLayout;
  param_tab_layout->setAlignment(Qt::AlignCenter);

  param_tab_layout->addWidget(label_lev, 0, 1, Qt::AlignCenter);
  param_tab_layout->addWidget(label_mean, 0, 2, Qt::AlignCenter);
  param_tab_layout->addWidget(label_uev, 0, 3, Qt::AlignCenter);

  param_tab_layout->addWidget(label_tau_inc, 1, 0);
  param_tab_layout->addWidget(inc_lev, 1, 1, Qt::AlignCenter);
  param_tab_layout->addWidget(inc_mean, 1, 2, Qt::AlignCenter);
  param_tab_layout->addWidget(inc_uev, 1, 3, Qt::AlignCenter);

  param_tab_layout->addWidget(label_percentage_predetect, 2, 0);
  param_tab_layout->addWidget(percentage_predetection, 2, 2, Qt::AlignCenter);

  param_tab_layout->addWidget(label_tau_symp, 3, 0);
  param_tab_layout->addWidget(symp_lev, 3, 1, Qt::AlignCenter);
  param_tab_layout->addWidget(symp_mean, 3, 2, Qt::AlignCenter);
  param_tab_layout->addWidget(symp_uev, 3, 3, Qt::AlignCenter);

  param_tab_layout->addWidget(label_tau_post, 4, 0);
  // param_tab_layout->addWidget(post_lev, 4,1, Qt::AlignCenter);
  param_tab_layout->addWidget(post_mean, 4, 2, Qt::AlignCenter);
  // param_tab_layout->addWidget(post_uev, 4,3, Qt::AlignCenter);

  param_tab_layout->addWidget(label_pcr_sens, 5, 0);
  param_tab_layout->addWidget(pcr_sens, 5, 2, Qt::AlignCenter);

  param_tab_layout->addWidget(label_pcr_spec, 6, 0);
  param_tab_layout->addWidget(pcr_spec, 6, 2, Qt::AlignCenter);

  param_tab_layout->addWidget(
      new QLabel(tr("Antigen-test sensitivity, relative to PCR-test [%]")), 7,
      0);
  param_tab_layout->addWidget(rel_antigen_sens, 7, 2, Qt::AlignCenter);

  param_tab_layout->addWidget(label_asymp, 8, 0);
  param_tab_layout->addWidget(percentage_asymptomatic, 8, 2, Qt::AlignCenter);

  param_tab_layout->addWidget(reset_PushButton, 9, 3);

  param_tab_layout->setHorizontalSpacing(20);
  param_tab_layout->setSizeConstraint(QLayout::SetFixedSize);

  QWidget *widget = new QWidget;
  widget->setLayout(param_tab_layout);
  return widget;
}

QWidget *MainWindow::initialize_tab_prevalence() {
  QGroupBox *incidence_reports = new QGroupBox();
  incidence_reports->setTitle("Incidence reports");

  QGridLayout *incidence_reports_layout = new QGridLayout;
  incidence_reports_layout->setHorizontalSpacing(20);
  incidence_reports_layout->setSizeConstraint(QLayout::SetFixedSize);

  incidence_reports->setLayout(incidence_reports_layout);

  QLabel *header_incidence = new QLabel(tr("Incidence"));
  QLabel *header_value = new QLabel(tr("Value"));
  QLabel *header_unit = new QLabel(tr("Unit"));
  QLabel *header_percent_of_population =
      new QLabel(tr("Percent of population"));

  QLabel *label_week4 = new QLabel(tr("Week -4"));
  QLabel *label_week3 = new QLabel(tr("Week -3"));
  QLabel *label_week2 = new QLabel(tr("Week -2"));
  QLabel *label_week1 = new QLabel(tr("Week -1"));
  QLabel *label_week0 = new QLabel(tr("This week"));

  QSpinBox *week4 =
      create_parameter_SpinBox(this, 0, 100000, this->default_values["week4"]);
  QSpinBox *week3 =
      create_parameter_SpinBox(this, 0, 100000, this->default_values["week3"]);
  QSpinBox *week2 =
      create_parameter_SpinBox(this, 0, 100000, this->default_values["week2"]);
  QSpinBox *week1 =
      create_parameter_SpinBox(this, 0, 100000, this->default_values["week1"]);
  QSpinBox *week0 =
      create_parameter_SpinBox(this, 0, 100000, this->default_values["week0"]);

  QLabel *label_unit4 = new QLabel(tr("1/100,000 individuals per week"));
  QLabel *label_unit3 = new QLabel(tr("1/100,000 individuals per week"));
  QLabel *label_unit2 = new QLabel(tr("1/100,000 individuals per week"));
  QLabel *label_unit1 = new QLabel(tr("1/100,000 individuals per week"));
  QLabel *label_unit0 = new QLabel(tr("1/100,000 individuals per week"));

  QLabel *pop_week4 = new QLabel(
      QString::number((float)this->default_values["week4"] / 100000 * 100, 'f',
                      2) +
      "%");
  QLabel *pop_week3 = new QLabel(
      QString::number((float)this->default_values["week3"] / 100000 * 100, 'f',
                      2) +
      "%");
  QLabel *pop_week2 = new QLabel(
      QString::number((float)this->default_values["week2"] / 100000 * 100, 'f',
                      2) +
      "%");
  QLabel *pop_week1 = new QLabel(
      QString::number((float)this->default_values["week1"] / 100000 * 100, 'f',
                      2) +
      "%");
  QLabel *pop_week0 = new QLabel(
      QString::number((float)this->default_values["week0"] / 100000 * 100, 'f',
                      2) +
      "%");

  incidence_reports_layout->addWidget(header_incidence, 0, 0, Qt::AlignCenter);
  incidence_reports_layout->addWidget(header_value, 0, 1, Qt::AlignCenter);
  incidence_reports_layout->addWidget(header_unit, 0, 2, Qt::AlignCenter);
  incidence_reports_layout->addWidget(header_percent_of_population, 0, 3,
                                      Qt::AlignCenter);

  incidence_reports_layout->addWidget(label_week4, 1, 0);
  incidence_reports_layout->addWidget(label_week3, 2, 0);
  incidence_reports_layout->addWidget(label_week2, 3, 0);
  incidence_reports_layout->addWidget(label_week1, 4, 0);
  incidence_reports_layout->addWidget(label_week0, 5, 0);

  incidence_reports_layout->addWidget(week4, 1, 1, Qt::AlignCenter);
  incidence_reports_layout->addWidget(week3, 2, 1, Qt::AlignCenter);
  incidence_reports_layout->addWidget(week2, 3, 1, Qt::AlignCenter);
  incidence_reports_layout->addWidget(week1, 4, 1, Qt::AlignCenter);
  incidence_reports_layout->addWidget(week0, 5, 1, Qt::AlignCenter);

  incidence_reports_layout->addWidget(label_unit4, 1, 2, Qt::AlignLeft);
  incidence_reports_layout->addWidget(label_unit3, 2, 2, Qt::AlignLeft);
  incidence_reports_layout->addWidget(label_unit2, 3, 2, Qt::AlignLeft);
  incidence_reports_layout->addWidget(label_unit1, 4, 2, Qt::AlignLeft);
  incidence_reports_layout->addWidget(label_unit0, 5, 2, Qt::AlignLeft);

  incidence_reports_layout->addWidget(pop_week4, 1, 3, Qt::AlignCenter);
  incidence_reports_layout->addWidget(pop_week3, 2, 3, Qt::AlignCenter);
  incidence_reports_layout->addWidget(pop_week2, 3, 3, Qt::AlignCenter);
  incidence_reports_layout->addWidget(pop_week1, 4, 3, Qt::AlignCenter);
  incidence_reports_layout->addWidget(pop_week0, 5, 3, Qt::AlignCenter);

  //--------------

  connect(week4, QOverload<int>::of(&QSpinBox::valueChanged), [=](int val) {
    float value = (float)val;
    pop_week4->setText(QString::number(value / 100000 * 100, 'f', 3) + "%");
  });
  connect(week3, QOverload<int>::of(&QSpinBox::valueChanged), [=](int val) {
    float value = (float)val;
    pop_week3->setText(QString::number(value / 100000 * 100, 'f', 3) + "%");
  });
  connect(week2, QOverload<int>::of(&QSpinBox::valueChanged), [=](int val) {
    float value = (float)val;
    pop_week2->setText(QString::number(value / 100000 * 100, 'f', 3) + "%");
  });
  connect(week1, QOverload<int>::of(&QSpinBox::valueChanged), [=](int val) {
    float value = (float)val;
    pop_week1->setText(QString::number(value / 100000 * 100, 'f', 3) + "%");
  });
  connect(week0, QOverload<int>::of(&QSpinBox::valueChanged), [=](int val) {
    float value = (float)val;
    pop_week0->setText(QString::number(value / 100000 * 100, 'f', 3) + "%");
  });

  QDoubleSpinBox *percent_detected = create_parameter_DoubleSpinBox(
      this, 0., 100., 2, this->default_values["percent_detected"]);

  QPushButton *calc_prevalence_PushButton = new QPushButton(this);
  calc_prevalence_PushButton->setText(tr("Estimate prevalence"));
  calc_prevalence_PushButton->setSizePolicy(QSizePolicy::Maximum,
                                            QSizePolicy::Maximum);

  QFormLayout *formLayout = new QFormLayout;
  formLayout->setSizeConstraint(QLayout::SetFixedSize);
  formLayout->addRow(tr("Percent of cases detected [%]  "), percent_detected);
  formLayout->addRow(calc_prevalence_PushButton);

  //------------------------------------

  QGroupBox *result_box = new QGroupBox();
  result_box->setTitle("Today:");
  result_box->setVisible(false);

  QSizePolicy sp_retain = result_box->sizePolicy();
  sp_retain.setRetainSizeWhenHidden(true);
  result_box->setSizePolicy(sp_retain);

  QGridLayout *result_gridLayout = new QGridLayout;
  result_gridLayout->setHorizontalSpacing(20);
  result_gridLayout->setSizeConstraint(QLayout::SetFixedSize);

  result_box->setLayout(result_gridLayout);

  QFont *bold_font = new QFont();
  bold_font->setBold(true);

  QLabel *header_predetect = new QLabel(tr("pre-detection \npre-infectious"));
  QLabel *header_presympt = new QLabel(tr("pre-symptomatic\ninfectious"));
  QLabel *header_sympt = new QLabel(tr("symptomatic\ninfectious"));
  QLabel *header_postsympt = new QLabel(tr("symptomatic\npost-infectious"));
  QLabel *header_total_prevalence = new QLabel(tr("prevalence\ntotal"));
  QLabel *header_prevalence = new QLabel(tr("prevalence\n(pre-)infectious"));

  QLabel *result_predetect_bc = new QLabel(tr(""));
  QLabel *result_presympt_bc = new QLabel(tr(""));
  QLabel *result_sympt_bc = new QLabel(tr(""));
  QLabel *result_postsympt_bc = new QLabel(tr(""));
  QLabel *result_total_prevalence_bc = new QLabel(tr(""));
  QLabel *result_prevalence_bc = new QLabel(tr(""));

  QLabel *range_predetect_bc = new QLabel(tr(""));
  QLabel *range_presympt_bc = new QLabel(tr(""));
  QLabel *range_sympt_bc = new QLabel(tr(""));
  QLabel *range_postsympt_bc = new QLabel(tr(""));
  QLabel *range_total_prevalence_bc = new QLabel(tr(""));
  QLabel *range_prevalence_bc = new QLabel(tr(""));

  result_gridLayout->addWidget(header_predetect, 0, 1, Qt::AlignCenter);
  result_gridLayout->addWidget(header_presympt, 0, 2, Qt::AlignCenter);
  result_gridLayout->addWidget(header_sympt, 0, 3, Qt::AlignCenter);
  result_gridLayout->addWidget(header_postsympt, 0, 4, Qt::AlignCenter);
  result_gridLayout->addWidget(header_total_prevalence, 0, 5, Qt::AlignCenter);
  result_gridLayout->addWidget(header_prevalence, 0, 6, Qt::AlignCenter);

  result_gridLayout->addWidget(result_predetect_bc, 1, 1, Qt::AlignCenter);
  result_gridLayout->addWidget(result_presympt_bc, 1, 2, Qt::AlignCenter);
  result_gridLayout->addWidget(result_sympt_bc, 1, 3, Qt::AlignCenter);
  result_gridLayout->addWidget(result_postsympt_bc, 1, 4, Qt::AlignCenter);
  result_gridLayout->addWidget(result_total_prevalence_bc, 1, 5,
                               Qt::AlignCenter);
  result_gridLayout->addWidget(result_prevalence_bc, 1, 6, Qt::AlignCenter);

  result_gridLayout->addWidget(range_predetect_bc, 2, 1, Qt::AlignCenter);
  result_gridLayout->addWidget(range_presympt_bc, 2, 2, Qt::AlignCenter);
  result_gridLayout->addWidget(range_sympt_bc, 2, 3, Qt::AlignCenter);
  result_gridLayout->addWidget(range_postsympt_bc, 2, 4, Qt::AlignCenter);
  result_gridLayout->addWidget(range_total_prevalence_bc, 2, 5,
                               Qt::AlignCenter);
  result_gridLayout->addWidget(range_prevalence_bc, 2, 6, Qt::AlignCenter);

  std::vector<QCheckBox *> v;
  for (int i = 0; i < 3; ++i) {
    QCheckBox *checkbox = new QCheckBox;
    checkbox->setChecked(false);
    checkbox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    v.push_back(checkbox);
  }

  connect(calc_prevalence_PushButton, &QPushButton::clicked, [=]() {
    std::vector<float> incidences{float(week0->value()) / 100000 /
                                      (float(percent_detected->value()) / 100),
                                  float(week1->value()) / 100000 /
                                      (float(percent_detected->value()) / 100),
                                  float(week2->value()) / 100000 /
                                      (float(percent_detected->value()) / 100),
                                  float(week3->value()) / 100000 /
                                      (float(percent_detected->value()) / 100),
                                  float(week4->value()) / 100000 /
                                      (float(percent_detected->value()) / 100)};

    Simulation *simulation = new Simulation(this);
    // tuple(states, probs)
    std::tuple<std::vector<Eigen::MatrixXf>, std::vector<Eigen::MatrixXf>>
        result = simulation->run_prevalence(incidences);
    this->result_prevalence_estimation = result;

    result_predetect_bc->setText(
        QString::number(std::get<1>(result)[0](0) * 100, 'f', 3) + "%");
    range_predetect_bc->setText(
        "(" + QString::number(std::get<1>(result)[1](0) * 100, 'f', 3) + ", " +
        QString::number(std::get<1>(result)[2](0) * 100, 'f', 3) + ")");
    result_presympt_bc->setText(
        QString::number(std::get<1>(result)[0](1) * 100, 'f', 3) + "%");
    range_presympt_bc->setText(
        "(" + QString::number(std::get<1>(result)[1](1) * 100, 'f', 3) + ", " +
        QString::number(std::get<1>(result)[2](1) * 100, 'f', 3) + ")");
    result_sympt_bc->setText(
        QString::number(std::get<1>(result)[0](2) * 100, 'f', 3) + "%");
    range_sympt_bc->setText(
        "(" + QString::number(std::get<1>(result)[1](2) * 100, 'f', 3) + ", " +
        QString::number(std::get<1>(result)[2](2) * 100, 'f', 3) + ")");
    result_postsympt_bc->setText(
        QString::number(std::get<1>(result)[0](3) * 100, 'f', 3) + "%");
    range_postsympt_bc->setText(
        "(" + QString::number(std::get<1>(result)[1](3) * 100, 'f', 3) + ", " +
        QString::number(std::get<1>(result)[2](3) * 100, 'f', 3) + ")");

    result_total_prevalence_bc->setText(
        QString::number((std::get<1>(result)[0](0) + std::get<1>(result)[0](1) +
                         std::get<1>(result)[0](2) +
                         std::get<1>(result)[0](3)) *
                            100,
                        'f', 3) +
        "%");
    range_total_prevalence_bc->setText(
        "(" +
        QString::number((std::get<1>(result)[1](0) + std::get<1>(result)[1](1) +
                         std::get<1>(result)[1](2) +
                         std::get<1>(result)[1](3)) *
                            100,
                        'f', 3) +
        ", " +
        QString::number((std::get<1>(result)[2](0) + std::get<1>(result)[2](1) +
                         std::get<1>(result)[2](2) +
                         std::get<1>(result)[2](3)) *
                            100,
                        'f', 3) +
        ")");

    result_prevalence_bc->setText(
        QString::number((std::get<1>(result)[0](0) + std::get<1>(result)[0](1) +
                         std::get<1>(result)[0](2)) *
                            100,
                        'f', 3) +
        "%");
    range_prevalence_bc->setText(
        "(" +
        QString::number((std::get<1>(result)[1](0) + std::get<1>(result)[1](1) +
                         std::get<1>(result)[1](2)) *
                            100,
                        'f', 3) +
        ", " +
        QString::number((std::get<1>(result)[2](0) + std::get<1>(result)[2](1) +
                         std::get<1>(result)[2](2)) *
                            100,
                        'f', 3) +
        ")");

    result_box->setVisible(true);

    this->scroll_tab->verticalScrollBar()->setValue(
        this->scroll_tab->verticalScrollBar()->maximum());

    for (int j = 0; j < 3; ++j) {
      v[j]->setChecked(false);
    }
    this->run_PushButton->setEnabled(false);
  });

  //-------------------------

  for (int i = 0; i < 3; ++i) {
    connect(v[i], &QCheckBox::stateChanged, [=](int) {
      if (v[i]->isChecked()) {
        for (int j = 0; j < 3; ++j) {
          if (j != i) {
            v[j]->setChecked(false);
          }
        }
        this->use_prevalence_estimation = true;
        this->initial_states =
            std::get<0>(this->result_prevalence_estimation)[i];
        this->pre_procedure_risk =
            std::get<1>(this->result_prevalence_estimation)[i](0) +
            std::get<1>(this->result_prevalence_estimation)[i](1) +
            std::get<1>(this->result_prevalence_estimation)[i](2);
        this->pre_risk->setValue(pre_procedure_risk);
        this->mode_ComboBox->setCurrentIndex(2);
        this->run_PushButton->setEnabled(true);
        this->tab->setCurrentIndex(0);
        this->scroll_tab->verticalScrollBar()->setValue(
            this->scroll_tab->verticalScrollBar()->minimum());
      }
    });
  }

  connect(mode_ComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int state) {
            if (state != 2) {
              for (auto box : v) {
                box->setChecked(false);
              }
            }
          });

  result_gridLayout->addWidget(new QLabel(tr("Use")), 0, 7, 1, 2,
                               Qt::AlignCenter);
  result_gridLayout->addWidget(v[0], 1, 7, 1, 2, Qt::AlignCenter);
  result_gridLayout->addWidget(v[1], 2, 7, Qt::AlignCenter);
  result_gridLayout->addWidget(v[2], 2, 8, Qt::AlignCenter);

  QVBoxLayout *prevalence_tab_layout = new QVBoxLayout;
  prevalence_tab_layout->setSizeConstraint(QLayout::SetFixedSize);
  prevalence_tab_layout->setAlignment(Qt::AlignTop);

  prevalence_tab_layout->addWidget(incidence_reports);
  prevalence_tab_layout->addItem(formLayout);
  prevalence_tab_layout->addWidget(result_box);

  QWidget *widget = new QWidget;
  widget->setLayout(prevalence_tab_layout);
  return widget;
}

void MainWindow::initialize_test_date_checkboxes() {
  // calc number of boxes needed
  int passed_days{0};
  passed_days = std::ceil(this->time_passed->value());
  int max_n_test = this->quarantine->value() + passed_days + 1;

  // create boxes
  this->test_date_checkboxes.clear();
  QHBoxLayout *layout = new QHBoxLayout;
  for (int i = -passed_days; i < max_n_test - passed_days; ++i) {
    QCheckBox *box = new QCheckBox();
    if (i < 0) {
      box->setEnabled(false);
    } else {
      this->test_date_checkboxes.push_back(box);
      if (i < int(this->test_date_checkboxes_states.size())) {
        box->setChecked(this->test_date_checkboxes_states.at(i));
      }
    }

    QVBoxLayout *vbox = new QVBoxLayout;
    QLabel *label = new QLabel(QString::number(i));
    vbox->addWidget(box);
    vbox->addWidget(label);
    vbox->setAlignment(Qt::AlignTop);

    layout->addItem(vbox);
  }
  layout->addStretch();
  this->test_days_box->setLayout(layout);
  this->test_date_checkboxes_states.clear();
}

void MainWindow::update_test_date_checkboxes() {
  // save states
  for (auto box : this->test_date_checkboxes) {
    if (box->isEnabled())
      this->test_date_checkboxes_states.push_back(box->isChecked());
  }
  // delete
  QLayout *hb = this->test_days_box->layout();
  while (!hb->isEmpty()) {
    QLayout *vb = hb->takeAt(0)->layout();
    while (!vb->isEmpty()) {
      QWidget *w = vb->takeAt(0)->widget();
      delete w;
    }
    delete vb;
  }
  delete hb;
  this->test_days_box->update();

  // re-initialize
  initialize_test_date_checkboxes();
}

void MainWindow::run_PushButton_clicked() {
  Simulation *simulation;
  if (use_prevalence_estimation) {
    simulation = new Simulation(this, initial_states, pre_procedure_risk);
  } else {
    simulation = new Simulation(this);
  }

  simulation->run();
  simulation->output_results();
}

void MainWindow::reset_PushButton_clicked() {
  this->inc_lev->setValue(this->default_values["inc_lev"]);
  this->inc_mean->setValue(this->default_values["inc_mean"]);
  this->inc_uev->setValue(this->default_values["inc_uev"]);

  this->percentage_predetection->setValue(
      this->default_values["percentage_predetection"]);

  this->symp_lev->setValue(this->default_values["symp_lev"]);
  this->symp_mean->setValue(this->default_values["symp_mean"]);
  this->symp_uev->setValue(this->default_values["symp_uev"]);

  this->percentage_asymptomatic->setValue(
      this->default_values["percentage_asymptomatic"]);

  this->post_lev->setValue(this->default_values["post_lev"]);
  this->post_mean->setValue(this->default_values["post_mean"]);
  this->post_uev->setValue(this->default_values["post_uev"]);

  this->pcr_sens->setValue(this->default_values["pcr_sens"]);
  this->pcr_spec->setValue(this->default_values["pcr_spec"]);
}

void MainWindow::time_passed_valueChanged() { update_test_date_checkboxes(); }

void MainWindow::quarantine_valueChanged() { update_test_date_checkboxes(); }

void MainWindow::mode_ComboBox_currentIndexChanged(int) {
  QLayout *strategy_tab_layout =
      this->tab->widget(0)->layout()->itemAt(0)->layout();
  QGridLayout *gridLayout = qobject_cast<QGridLayout *>(strategy_tab_layout);
  QWidget *widget = gridLayout->itemAtPosition(2, 0)->widget();
  QLabel *label_time_passed = qobject_cast<QLabel *>(widget);

  label_time_passed->setText(
      (std::string{"Time passed since "} +
       this->mode_map_int[this->mode_ComboBox->currentIndex()] +
       std::string{" [days]:"})
          .c_str());

  this->use_symptomatic_screening->setChecked(true);
  this->use_symptomatic_screening->setEnabled(true);
  gridLayout->itemAtPosition(4, 0)->widget()->setEnabled(true);

  this->pre_risk->setEnabled(true);
  this->pre_risk->setValue(1);

  if (this->mode_ComboBox->currentIndex() == 2) {
    this->time_passed->setValue(0);
    this->time_passed->setEnabled(false);
    this->pre_risk->setEnabled(false);
    if (!this->use_prevalence_estimation) {
      this->run_PushButton->setEnabled(false);
    }
    this->tab->setCurrentIndex(2);
    this->scroll_tab->verticalScrollBar()->setValue(
        this->scroll_tab->verticalScrollBar()->maximum());
  } else {
    this->use_prevalence_estimation = false;
    this->time_passed->setEnabled(true);
    this->run_PushButton->setEnabled(true);
  }

  if (this->mode_ComboBox->currentIndex() == 1) // system onset mode
  {
    this->use_symptomatic_screening->setChecked(false);
    this->use_symptomatic_screening->setEnabled(false);
    gridLayout->itemAtPosition(4, 0)->widget()->setEnabled(false);
  }
}
