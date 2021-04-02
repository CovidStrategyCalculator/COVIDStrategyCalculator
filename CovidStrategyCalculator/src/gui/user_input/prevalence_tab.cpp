/* prevalence_tab.cpp
 * Written by Wiep van der Toorn.
 *
 * This file is part of COVIDStrategycalculator.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 *
 * This file implements the PrevalenceTab class for the prevalence-related user input which derives from QWidget.
 * The PrevalenceTab provides the third tab of the user input field and acts as the interface to the prevalence
 * estimator feature of the COVIDStrategycalculator.
 * This widget allows users to run a prevalence estimation based on the recent incidence reports of a region/country and
 * to use the results (a mix of ‘infection ages’) as the initital states of the main simulation.
 */

#include "include/gui/user_input/prevalence_tab.h"
#include "include/core/model.h"
#include "include/gui/utils.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>

PrevalenceTab::PrevalenceTab(QWidget *parent) : QWidget(parent) {
    initialize_member_variables();
    set_layout();
}

void PrevalenceTab::initialize_member_variables() {

    reports4_ = Utils::create_SpinBox(50, 0, 100000);
    reports3_ = Utils::create_SpinBox(50, 0, 100000);
    reports2_ = Utils::create_SpinBox(50, 0, 100000);
    reports1_ = Utils::create_SpinBox(50, 0, 100000);
    reports0_ = Utils::create_SpinBox(50, 0, 100000);

    char tt_reports4_[] = "<html><head/><body><p> "
                          "Four weeks prior to current week."
                          "</p></body></html>";
    reports4_->setToolTip(tt_reports4_);
    char tt_reports3_[] = "<html><head/><body><p> "
                          "Three weeks prior to current week."
                          "</p></body></html>";
    reports3_->setToolTip(tt_reports3_);
    char tt_reports2_[] = "<html><head/><body><p> "
                          "Two weeks prior to current week."
                          "</p></body></html>";
    reports2_->setToolTip(tt_reports2_);
    char tt_reports1_[] = "<html><head/><body><p> "
                          "One week prior to current week."
                          "</p></body></html>";
    reports1_->setToolTip(tt_reports1_);
    char tt_reports0_[] = "<html><head/><body><p> "
                          "Current week: past 7 days."
                          "</p></body></html>";
    reports0_->setToolTip(tt_reports0_);

    percent_of_population4_ = new QLabel(QString::number((float)reports4_->value() / 100000. * 100., 'f', 3) + "%");
    percent_of_population3_ = new QLabel(QString::number((float)reports3_->value() / 100000. * 100., 'f', 3) + "%");
    percent_of_population2_ = new QLabel(QString::number((float)reports2_->value() / 100000. * 100., 'f', 3) + "%");
    percent_of_population1_ = new QLabel(QString::number((float)reports1_->value() / 100000. * 100., 'f', 3) + "%");
    percent_of_population0_ = new QLabel(QString::number((float)reports0_->value() / 100000. * 100., 'f', 3) + "%");

    percent_of_cases_detected_ = Utils::create_DoubleSpinBox(10, 0.01, 100, 2);

    run_button_ = new QPushButton(tr("Estimate prevalence"));
    run_button_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(run_button_, &QPushButton::clicked, [=]() { estimate_prevalence(); });

    result_box_ = new QGroupBox("Estimated state today:");

    result_predetect_ = new QLabel(tr(" "));
    result_presympt_ = new QLabel(tr(" "));
    result_sympt_ = new QLabel(tr(" "));
    result_postinf_ = new QLabel(tr(" "));
    result_total_prevalence_ = new QLabel(tr(" "));
    result_inf_prevalence_ = new QLabel(tr(" "));
    result_predetect_range_ = new QLabel(tr(" "));
    result_presympt_range_ = new QLabel(tr(" "));
    result_sympt_range_ = new QLabel(tr(" "));
    result_postinf_range_ = new QLabel(tr(" "));
    result_total_prevalence_range_ = new QLabel(tr(" "));
    result_inf_prevalence_range_ = new QLabel(tr(" "));

    QString tt_cb_typical = tr("<html><head/><body><p> "
                               "Prevalence estimation based on typical case dynamics. "
                               "Select to use these results as the inital conditions "
                               "for a simulation in 'incoming travelers' mode."
                               "</p></body></html>");
    QString tt_cb_best = tr("<html><head/><body><p> "
                            "Prevalence estimation based on extreme case dynamics, best case. "
                            "Select to use these results as the inital conditions "
                            "for a simulation in 'incoming travelers' mode."
                            "</p></body></html>");
    QString tt_cb_worst = tr("<html><head/><body><p> "
                             "Prevalence estimation based on extreme case dynamics, worst case. "
                             "Select to use these results as the inital conditions "
                             "for a simulation in 'incoming travelers' mode."
                             "</p></body></html>");
    std::vector<QString> tooltips_checkboxes{tt_cb_typical, tt_cb_best, tt_cb_worst};

    for (int i = 0; i < 3; ++i) {
        QCheckBox *checkbox = new QCheckBox;
        checkbox->setChecked(false);
        checkbox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        checkbox->setToolTip(tooltips_checkboxes[i]);
        use_for_main_simulation_.push_back(checkbox);
    }

    for (int i = 0; i < 3; ++i) {
        connect(use_for_main_simulation_[i], &QCheckBox::stateChanged, [=]() { state_of_use_checkbox_changed(); });
    }

    connect(reports4_, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        percent_of_population4_->setText(Utils::safeguard_probability((float)value / 100000. * 100., 3) + "%");
    });
    connect(reports3_, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        percent_of_population3_->setText(Utils::safeguard_probability((float)value / 100000. * 100., 3) + "%");
    });
    connect(reports2_, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        percent_of_population2_->setText(Utils::safeguard_probability((float)value / 100000. * 100., 3) + "%");
    });
    connect(reports1_, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        percent_of_population1_->setText(Utils::safeguard_probability((float)value / 100000. * 100., 3) + "%");
    });
    connect(reports0_, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        percent_of_population0_->setText(Utils::safeguard_probability((float)value / 100000. * 100., 3) + "%");
    });
}

void PrevalenceTab::state_of_use_checkbox_changed() {
    for (int idx = 0; idx < 3; ++idx) {
        if (use_for_main_simulation_[idx]->isChecked()) {
            for (int other_idx = 0; other_idx < 3; ++other_idx) {
                if (other_idx != idx) {
                    use_for_main_simulation_[other_idx]->setChecked(false); // only one box can be checked
                }
            }
            use_case = idx; // mark which case is to be used in the main simulation
            use_prevalence_estimation_ = true;
            emit use_checkbox_checked();
            emit enable_main_run_button();
        }
    }
    if (((!use_for_main_simulation_[0]->isChecked() && !use_for_main_simulation_[1]->isChecked()) &&
         !use_for_main_simulation_[2]->isChecked())) {
        emit all_boxes_are_unchecked(); // prevent undefined behaviour when no box is checked in incoming travelers
    }
}

void PrevalenceTab::uncheck_all_use_case_boxes() {
    use_prevalence_estimation_ = false;
    for (auto checkbox : use_for_main_simulation_) {
        checkbox->setChecked(false);
    }
}

void PrevalenceTab::set_layout() {
    // incidence history
    QGroupBox *incidence_history_box = new QGroupBox("Incidence history");
    QGridLayout *incidence_layout = new QGridLayout;
    incidence_history_box->setLayout(incidence_layout);

    incidence_layout->addWidget(new QLabel(tr("Incidence")), 0, 0, Qt::AlignCenter);
    incidence_layout->addWidget(new QLabel(tr("Value")), 0, 1, Qt::AlignCenter);
    incidence_layout->addWidget(new QLabel(tr("Unit")), 0, 2, Qt::AlignCenter);
    incidence_layout->addWidget(new QLabel(tr("Percent of population")), 0, 3, Qt::AlignCenter);

    QLabel *label_week4 = new QLabel(tr("Week -4"));
    char tt_label_week4[] = "<html><head/><body><p> "
                            "Four weeks prior to current week."
                            "</p></body></html>";
    label_week4->setToolTip(tt_label_week4);

    QLabel *label_week3 = new QLabel(tr("Week -3"));
    char tt_label_week3[] = "<html><head/><body><p> "
                            "Three weeks prior to current week."
                            "</p></body></html>";
    label_week3->setToolTip(tt_label_week3);

    QLabel *label_week2 = new QLabel(tr("Week -2"));
    char tt_label_week2[] = "<html><head/><body><p> "
                            "Two weeks prior to current week."
                            "</p></body></html>";
    label_week2->setToolTip(tt_label_week2);

    QLabel *label_week1 = new QLabel(tr("Week -1"));
    char tt_label_week1[] = "<html><head/><body><p> "
                            "One week prior to current week."
                            "</p></body></html>";
    label_week1->setToolTip(tt_label_week1);

    QLabel *label_week0 = new QLabel(tr("This week"));
    char tt_label_week0[] = "<html><head/><body><p> "
                            "Current week: past 7 days."
                            "</p></body></html>";
    label_week0->setToolTip(tt_label_week0);

    incidence_layout->addWidget(label_week4, 5, 0);
    incidence_layout->addWidget(label_week3, 4, 0);
    incidence_layout->addWidget(label_week2, 3, 0);
    incidence_layout->addWidget(label_week1, 2, 0);
    incidence_layout->addWidget(label_week0, 1, 0);

    incidence_layout->addWidget(reports4_, 5, 1, Qt::AlignCenter);
    incidence_layout->addWidget(reports3_, 4, 1, Qt::AlignCenter);
    incidence_layout->addWidget(reports2_, 3, 1, Qt::AlignCenter);
    incidence_layout->addWidget(reports1_, 2, 1, Qt::AlignCenter);
    incidence_layout->addWidget(reports0_, 1, 1, Qt::AlignCenter);

    incidence_layout->addWidget(new QLabel(tr("1/100,000 individuals per week")), 1, 2, Qt::AlignLeft);
    incidence_layout->addWidget(new QLabel(tr("1/100,000 individuals per week")), 2, 2, Qt::AlignLeft);
    incidence_layout->addWidget(new QLabel(tr("1/100,000 individuals per week")), 3, 2, Qt::AlignLeft);
    incidence_layout->addWidget(new QLabel(tr("1/100,000 individuals per week")), 4, 2, Qt::AlignLeft);
    incidence_layout->addWidget(new QLabel(tr("1/100,000 individuals per week")), 5, 2, Qt::AlignLeft);

    incidence_layout->addWidget(percent_of_population4_, 1, 3, Qt::AlignCenter);
    incidence_layout->addWidget(percent_of_population3_, 2, 3, Qt::AlignCenter);
    incidence_layout->addWidget(percent_of_population2_, 3, 3, Qt::AlignCenter);
    incidence_layout->addWidget(percent_of_population1_, 4, 3, Qt::AlignCenter);
    incidence_layout->addWidget(percent_of_population0_, 5, 3, Qt::AlignCenter);

    incidence_layout->setHorizontalSpacing(10);
    incidence_layout->setSizeConstraint(QLayout::SetFixedSize);

    // effort esttimate and run button
    QFormLayout *form_layout = new QFormLayout;
    form_layout->setSizeConstraint(QLayout::SetFixedSize);
    form_layout->addRow(tr("Percent of cases detected [%]  "), percent_of_cases_detected_);
    form_layout->addRow(run_button_);

    // results
    QGridLayout *result_layout = new QGridLayout;

    result_layout->addWidget(new QLabel(tr("pre-detection \npre-infectious")), 0, 1, Qt::AlignCenter);
    result_layout->addWidget(new QLabel(tr("pre-symptomatic\ninfectious")), 0, 2, Qt::AlignCenter);
    result_layout->addWidget(new QLabel(tr("symptomatic\ninfectious")), 0, 3, Qt::AlignCenter);
    result_layout->addWidget(new QLabel(tr("symptomatic\npost-infectious")), 0, 4, Qt::AlignCenter);
    result_layout->addWidget(new QLabel(tr("prevalence\ntotal")), 0, 5, Qt::AlignCenter);
    result_layout->addWidget(new QLabel(tr("prevalence\n(pre-)infectious")), 0, 6, Qt::AlignCenter);

    result_layout->addWidget(result_predetect_, 1, 1, Qt::AlignCenter);
    result_layout->addWidget(result_presympt_, 1, 2, Qt::AlignCenter);
    result_layout->addWidget(result_sympt_, 1, 3, Qt::AlignCenter);
    result_layout->addWidget(result_postinf_, 1, 4, Qt::AlignCenter);
    result_layout->addWidget(result_total_prevalence_, 1, 5, Qt::AlignCenter);
    result_layout->addWidget(result_inf_prevalence_, 1, 6, Qt::AlignCenter);

    result_layout->addWidget(result_predetect_range_, 2, 1, Qt::AlignCenter);
    result_layout->addWidget(result_presympt_range_, 2, 2, Qt::AlignCenter);
    result_layout->addWidget(result_sympt_range_, 2, 3, Qt::AlignCenter);
    result_layout->addWidget(result_postinf_range_, 2, 4, Qt::AlignCenter);
    result_layout->addWidget(result_total_prevalence_range_, 2, 5, Qt::AlignCenter);
    result_layout->addWidget(result_inf_prevalence_range_, 2, 6, Qt::AlignCenter);

    QLabel *label_use = new QLabel(tr("Use"));
    char tt_label_use[] = "<html><head/><body><p>"
                          "Please select one of the checkboxes below if you want to use the "
                          "results for a simulation in 'incoming travelers' mode."
                          "</p></body></html>";
    label_use->setToolTip(tt_label_use);
    result_layout->addWidget(label_use, 0, 7, 1, 2, Qt::AlignCenter);
    result_layout->addWidget(use_for_main_simulation_[0], 1, 7, 1, 2, Qt::AlignCenter);
    result_layout->addWidget(use_for_main_simulation_[1], 2, 7, Qt::AlignCenter);
    result_layout->addWidget(use_for_main_simulation_[2], 2, 8, Qt::AlignCenter);

    result_layout->setHorizontalSpacing(10);
    result_layout->setSizeConstraint(QLayout::SetFixedSize);

    result_box_->setLayout(result_layout);
    result_box_->setVisible(false);
    QSizePolicy sp_retain = result_box_->sizePolicy(); // update sizePolicy such that space is reserved in layout
    sp_retain.setRetainSizeWhenHidden(true);
    result_box_->setSizePolicy(sp_retain);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setSizeConstraint(QLayout::SetFixedSize);
    main_layout->setAlignment(Qt::AlignTop);

    main_layout->addWidget(incidence_history_box);
    main_layout->addItem(form_layout);
    main_layout->addWidget(result_box_);

    this->setLayout(main_layout);
}

// TODO reorder probabilites
void PrevalenceTab::update_layout() {

    Eigen::MatrixXf ordered = Utils::mid_min_max(phase_probabilities_ * 100);
    Eigen::VectorXf mid = ordered.col(0);
    Eigen::VectorXf min = ordered.col(1);
    Eigen::VectorXf max = ordered.col(2);

    result_predetect_->setText(Utils::safeguard_probability(mid[0], 3) + "%");
    result_presympt_->setText(Utils::safeguard_probability(mid[1], 3) + "%");
    result_sympt_->setText(Utils::safeguard_probability(mid[2], 3) + "%");
    result_postinf_->setText(Utils::safeguard_probability(mid[3], 3) + "%");
    result_total_prevalence_->setText(Utils::safeguard_probability(mid[0] + mid[1] + mid[2] + mid[3], 3) + "%");
    result_inf_prevalence_->setText(Utils::safeguard_probability(mid[0] + mid[1] + mid[2], 3) + "%");

    result_predetect_range_->setText("(" + Utils::safeguard_probability(min[0], 3) + ", " +
                                     Utils::safeguard_probability(max[0], 3) + ")");
    result_presympt_range_->setText("(" + Utils::safeguard_probability(min[1], 3) + ", " +
                                    Utils::safeguard_probability(max[1], 3) + ")");
    result_sympt_range_->setText("(" + Utils::safeguard_probability(min[2], 3) + ", " +
                                 Utils::safeguard_probability(max[2], 3) + ")");
    result_postinf_range_->setText("(" + Utils::safeguard_probability(min[3], 3) + ", " +
                                   Utils::safeguard_probability(max[3], 3) + ")");
    result_total_prevalence_range_->setText("(" + Utils::safeguard_probability(min[0] + min[1] + min[2] + min[3], 3) +
                                            ", " + Utils::safeguard_probability(max[0] + max[1] + max[2] + max[3], 3) +
                                            ")");
    result_inf_prevalence_range_->setText("(" + Utils::safeguard_probability(min[0] + min[1] + min[2], 3) + ", " +
                                          Utils::safeguard_probability(max[0] + max[1] + max[2], 3) + ")");

    result_box_->setVisible(true);
}

void PrevalenceTab::estimate_prevalence() {
    emit run_prevalence_estimator();
    uncheck_all_use_case_boxes();
    emit disable_main_run_button();
}

std::vector<float> PrevalenceTab::incidence() {
    float effort_estimate = 100. / static_cast<float>(percent_of_cases_detected_->value());
    std::vector<float> incidences{static_cast<float>(reports0_->value()) * effort_estimate / float(100000.),
                                  static_cast<float>(reports1_->value()) * effort_estimate / float(100000.),
                                  static_cast<float>(reports2_->value()) * effort_estimate / float(100000.),
                                  static_cast<float>(reports3_->value()) * effort_estimate / float(100000.),
                                  static_cast<float>(reports4_->value()) * effort_estimate / float(100000.)};
    return incidences;
}
