/* strategy_tab.cpp
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
 * This file implements the StrategyTab class for the strategy-related user input and derives from QWidget.
 * The StrategyTab provides the first tab of the user input field and allows the user to design and evaluate a custom
 * NPI strategy.
 */

#include "include/gui/user_input/strategy_tab.h"
#include "include/gui/utils.h"

#include <QGridLayout>
#include <QLabel>

StrategyTab::StrategyTab(QWidget *parent) : QWidget(parent) {
    initialize_member_variables();
    set_layout();
}

void StrategyTab::initialize_member_variables() {
    mode_ = new QComboBox;
    mode_->addItems(QStringList{"contact management", "isolation", "incoming travelers"});
    mode_->setCurrentIndex(0);
    char tt_mode_[] = "<html><head/><body><p> Determines start situation of simulation: "
                      "contant management starts at time of exposure, "
                      "isolation starts at time of sympton onset, and "
                      "incoming travelers uses a mixed-age distribution based on a "
                      "selected prevalence estimation. "
                      "</p></body></html>";
    mode_->setToolTip(tt_mode_);

    test_type_ = new QComboBox;
    test_type_->addItems(QStringList{"PCR", "Antigen"});
    test_type_->setCurrentIndex(0);

    p_infectious_t0_ = Utils::create_DoubleSpinBox(1, 0, 1, 3);
    time_delay_ = Utils::create_SpinBox(0, 0, 21);
    end_of_strategy_ = Utils::create_SpinBox(10, 0, 35);
    expected_adherence_ = Utils::create_SpinBox(100, 0, 100);
    char tt_expected_adherence_[] = "<html><head/><body><p> "
                                    "The expected level of adherence describes which percentage "
                                    "of individuals follows the proposed NPI strategy."
                                    "</p></body></html>";
    expected_adherence_->setToolTip(tt_expected_adherence_);

    use_symptomatic_screening_ = new QCheckBox;
    use_symptomatic_screening_->setChecked(true);
    char tt_use_symptomatic_screening_[] =
        "<html><head/><body><p> "
        "`Symptom screening’ implies that individuals who develop symptoms go into isolation "
        "and do not pose a risk anymore, whereas individuals that not develop symptoms are "
        "being released into society at the end of the quarantine and may continue to pose a risk. "
        "</p></body></html>";
    use_symptomatic_screening_->setToolTip(tt_use_symptomatic_screening_);

    test_days_box_ = new QGroupBox;
    test_days_box_->setTitle(tr("Days to test on:"));
    set_layout_test_days_box();

    run_button_ = new QPushButton(tr("Run"));
    run_button_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    connect(mode_, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        configure_options_based_on_mode(index);
        emit mode_changed();
    });
    connect(time_delay_, QOverload<int>::of(&QSpinBox::valueChanged), [=]() { update_test_days_box(); });
    connect(end_of_strategy_, QOverload<int>::of(&QSpinBox::valueChanged), [=]() { update_test_days_box(); });
    connect(run_button_, &QPushButton::clicked, [=]() { emit run_simulation(); });
}

void StrategyTab::set_layout() {
    QGridLayout *upper_grid_layout = new QGridLayout;

    QLabel *label_mode = new QLabel(tr("Simulation mode: "));
    char tt_label_mode[] = "<html><head/><body><p> Determines start situation of simulation: "
                           "contant management starts at time of exposure, "
                           "isolation starts at time of sympton onset, and "
                           "incoming travelers uses a mixed-age distribution based on a "
                           "preliminary prevalence estimation. "
                           "</p></body></html>";
    label_mode->setToolTip(tt_label_mode);
    upper_grid_layout->addWidget(label_mode, 0, 0);
    upper_grid_layout->addWidget(mode_, 0, 1, Qt::AlignLeft);

    upper_grid_layout->addWidget(new QLabel(tr("Initial probability of infection: ")), 1, 0);
    upper_grid_layout->addWidget(p_infectious_t0_, 1, 1, Qt::AlignLeft);

    upper_grid_layout->addWidget(new QLabel(tr("Time passed since exposure/symptom onset [days]: ")), 2, 0);
    upper_grid_layout->addWidget(time_delay_, 2, 1, Qt::AlignLeft);

    upper_grid_layout->addWidget(new QLabel(tr("Duration of quarantine/isolation [days]: ")), 3, 0);
    upper_grid_layout->addWidget(end_of_strategy_, 3, 1, Qt::AlignLeft);

    QLabel *label_symptom_screening = new QLabel(tr("Use symptom screening: "));
    char tt_label_symptom_screening[] =
        "<html><head/><body><p> "
        "`Symptom screening’ implies that individuals who develop symptoms go into isolation "
        "and do not pose a risk anymore, whereas individuals that not develop symptoms are "
        "being released into society at the end of the quarantine and may continue to pose a risk. "
        "</p></body></html>";
    label_symptom_screening->setToolTip(tt_label_symptom_screening);
    upper_grid_layout->addWidget(label_symptom_screening, 4, 0);
    upper_grid_layout->addWidget(use_symptomatic_screening_, 4, 1, Qt::AlignLeft);

    QLabel *label_expected_adherence = new QLabel(tr("Expected adherence [%]: "));
    char tt_label_expected_adherence[] = "<html><head/><body><p> "
                                         "The expected level of adherence describes which percentage "
                                         "of individuals follows the proposed NPI strategy."
                                         "</p></body></html>";
    label_expected_adherence->setToolTip(tt_label_expected_adherence);
    upper_grid_layout->addWidget(label_expected_adherence, 5, 0);
    upper_grid_layout->addWidget(expected_adherence_, 5, 1, Qt::AlignLeft);

    QLabel *logo = new QLabel;
    logo->setPixmap(QPixmap((":/logo.jpg")));
    upper_grid_layout->addWidget(logo, 0, 2, 3, 1);
    QSpacerItem *space = new QSpacerItem(1, 1, QSizePolicy::Expanding);
    upper_grid_layout->addItem(space, 0, 4, 1, 2, Qt::AlignLeft);

    upper_grid_layout->setHorizontalSpacing(10);
    upper_grid_layout->setSizeConstraint(QLayout::SetFixedSize);

    QHBoxLayout *upper_layout = new QHBoxLayout;
    upper_layout->addItem(upper_grid_layout);
    upper_layout->addStretch();

    QHBoxLayout *test_type_layout = new QHBoxLayout;
    test_type_layout->addWidget(new QLabel(tr("Type of test: ")));
    test_type_layout->addWidget(test_type_);
    test_type_layout->addStretch();
    test_type_layout->setSizeConstraint(QLayout::SetFixedSize);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addItem(upper_layout);
    main_layout->addWidget(test_days_box_);
    main_layout->addItem(test_type_layout);
    main_layout->addStretch();
    main_layout->addWidget(run_button_);
    main_layout->setSizeConstraint(QLayout::SetFixedSize);
    main_layout->setAlignment(Qt::AlignTop);

    this->setLayout(main_layout);
}

void StrategyTab::set_layout_test_days_box() {
    QHBoxLayout *main_layout = new QHBoxLayout;
    test_days_boxes.clear(); // clean sheet

    int n = time_delay() + end_of_strategy() + 1;
    for (int day = -time_delay(); day < n - time_delay(); ++day) {
        QCheckBox *box = new QCheckBox;
        if (day < 0) {
            box->setEnabled(false);
        } else {
            if (day < int(test_days_boxes_states.size())) {      // day was also part of previous strategy
                box->setChecked(test_days_boxes_states.at(day)); // retain previous state
            }
        }
        test_days_boxes.push_back(box);

        QVBoxLayout *vbox = new QVBoxLayout;
        vbox->addWidget(box);
        vbox->addWidget(new QLabel(QString::number(day)));
        vbox->setAlignment(Qt::AlignTop);

        main_layout->addItem(vbox);
    }

    main_layout->addStretch();
    test_days_box_->setLayout(main_layout);
    test_days_boxes_states.clear();
    this->resize(sizeHint());
}

void StrategyTab::update_test_days_box() {
    // save states of current strategy
    for (auto box : test_days_boxes) {
        test_days_boxes_states.push_back(box->isChecked());
    }
    // delete existing layout recursively
    QLayout *layout = test_days_box_->layout();
    while (!layout->isEmpty()) {
        QLayout *vbox = layout->takeAt(0)->layout();
        while (!vbox->isEmpty()) {
            QWidget *element = vbox->takeAt(0)->widget();
            delete element;
        }
        delete vbox;
    }
    delete layout;

    test_days_box_->update();
    set_layout_test_days_box();
};

void StrategyTab::configure_options_based_on_mode(int current_mode) {
    switch (current_mode) {
    case 0:
        use_symptomatic_screening_->setChecked(true);
        use_symptomatic_screening_->setEnabled(true);
        p_infectious_t0_->setEnabled(true);
        p_infectious_t0_->setValue(1);
        time_delay_->setEnabled(true);
        run_button_->setEnabled(true);
        break;
    case 1:
        use_symptomatic_screening_->setChecked(false);
        use_symptomatic_screening_->setEnabled(false);
        p_infectious_t0_->setEnabled(true);
        p_infectious_t0_->setValue(1);
        time_delay_->setEnabled(true);
        run_button_->setEnabled(true);
        break;
    case 2:
        use_symptomatic_screening_->setChecked(true);
        use_symptomatic_screening_->setEnabled(true);
        p_infectious_t0_->setEnabled(false);
        run_button_->setEnabled(false);
        time_delay_->setEnabled(false);
        time_delay_->setValue(0);
        emit jump_to_prevalence_tab();
        emit mode_is_2();
        break;
    }
}

// indices of placed tests; 0-indexed, also in case of non-zero time delay
std::vector<int> StrategyTab::test_moments() const {
    std::vector<int> v{};
    for (int i = 0; i < (int)test_days_boxes.size(); ++i) {
        if (test_days_boxes.at(i)->isChecked()) {
            v.push_back(i);
        }
    }
    return v;
}
