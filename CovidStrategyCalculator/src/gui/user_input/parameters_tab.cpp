/* parameters_tab.cpp
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
 * This file implements the ParametersTab class for the parameter-related user input which derives from QWidget.
 * The ParametersTab provides the second tab of the user input field and bundles together the model parameters of the
 * simulation. The parameters in this class are independent of the user-defined NPI strategy.
 */

#include "include/gui/user_input/parameters_tab.h"
#include "include/gui/utils.h"

#include <QGridLayout>
#include <QLabel>

ParametersTab::ParametersTab(QWidget *parent) : QWidget(parent) {
    initialize_member_variables();
    set_layout();
}

void ParametersTab::initialize_member_variables() {

    incubation_mean_ = Utils::create_DoubleSpinBox(default_values["duration_incubation_mean"], 0.01, 100, 2);
    incubation_upper_ = Utils::create_DoubleSpinBox(default_values["duration_incubation_upper"], 0.01, 100, 2);
    incubation_lower_ = Utils::create_DoubleSpinBox(default_values["duration_incubation_lower"], 0.01, 100, 2);
    percentage_predetection_ =
        Utils::create_DoubleSpinBox(default_values["percentage_of_incubation_predetection"], 0.01, 100, 2);
    symptomatic_mean_ = Utils::create_DoubleSpinBox(default_values["duration_symptomatic_mean"], 0.01, 100, 2);
    symptomatic_upper_ = Utils::create_DoubleSpinBox(default_values["duration_symptomatic_upper"], 0.01, 100, 2);
    symptomatic_lower_ = Utils::create_DoubleSpinBox(default_values["duration_symptomatic_lower"], 0.01, 100, 2);
    percentage_asymptomatic_ = Utils::create_DoubleSpinBox(default_values["percentage_asymptomatic"], 0.01, 100, 2);
    postinfectious_ = Utils::create_DoubleSpinBox(default_values["duration_postinfectious_mean"], 0.01, 100, 2);
    pcr_sens_ = Utils::create_DoubleSpinBox(default_values["PCR_sensitivity"], 0.01, 100, 2);
    pcr_spec_ = Utils::create_DoubleSpinBox(default_values["PCR_specificity"], 0.01, 100, 2);
    relative_rdt_sens_ = Utils::create_DoubleSpinBox(default_values["relative_RDT_sensitivity"], 0.01, 100, 2);

    reset_button_ = new QPushButton(tr("Reset defaults"));
    reset_button_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(reset_button_, &QPushButton::clicked, [=]() { reset_defaults(); });
}

void ParametersTab::set_layout() {
    QGridLayout *main_layout = new QGridLayout;
    main_layout->setAlignment(Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Lower extreme value")), 0, 1, Qt::AlignCenter);
    main_layout->addWidget(new QLabel(tr("Typical")), 0, 2, Qt::AlignCenter);
    main_layout->addWidget(new QLabel(tr("Upper extreme value")), 0, 3, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Mean duration of incubation period [day]:")), 1, 0);
    main_layout->addWidget(incubation_mean_, 1, 2, Qt::AlignCenter);
    main_layout->addWidget(incubation_upper_, 1, 3, Qt::AlignCenter);
    main_layout->addWidget(incubation_lower_, 1, 1, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Percentage thereof pre-detectable [%]:")), 2, 0);
    main_layout->addWidget(percentage_predetection_, 2, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Mean duration of symptomatic period [day]:")), 3, 0);
    main_layout->addWidget(symptomatic_mean_, 3, 2, Qt::AlignCenter);
    main_layout->addWidget(symptomatic_upper_, 3, 3, Qt::AlignCenter);
    main_layout->addWidget(symptomatic_lower_, 3, 1, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Mean duration of post-symptomatic, detectable period [day]:")), 4, 0);
    main_layout->addWidget(postinfectious_, 4, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("PCR-test sensitivity [%]")), 5, 0);
    main_layout->addWidget(pcr_sens_, 5, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("PCR-test specificity [%]")), 6, 0);
    main_layout->addWidget(pcr_spec_, 6, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Antigen-test sensitivity, relative to PCR-test [%]")), 7, 0);
    main_layout->addWidget(relative_rdt_sens_, 7, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Percentage of asymptomatic infections [%]:")), 8, 0);
    main_layout->addWidget(percentage_asymptomatic_, 8, 2, Qt::AlignCenter);

    main_layout->addWidget(reset_button_, 9, 3);

    main_layout->setHorizontalSpacing(10);
    main_layout->setSizeConstraint(QLayout::SetFixedSize);

    this->setLayout(main_layout);
}

void ParametersTab::reset_defaults() {
    incubation_mean_->setValue(default_values["duration_incubation_mean"]);
    incubation_upper_->setValue(default_values["duration_incubation_upper"]);
    incubation_lower_->setValue(default_values["duration_incubation_lower"]);
    percentage_predetection_->setValue(default_values["percentage_of_incubation_predetection"]);
    symptomatic_mean_->setValue(default_values["duration_symptomatic_mean"]);
    symptomatic_upper_->setValue(default_values["duration_symptomatic_upper"]);
    symptomatic_lower_->setValue(default_values["duration_symptomatic_lower"]);
    percentage_asymptomatic_->setValue(default_values["percentage_asymptomatic"]);
    postinfectious_->setValue(default_values["duration_postinfectious_mean"]);
    pcr_sens_->setValue(default_values["PCR_sensitivity"]);
    pcr_spec_->setValue(default_values["PCR_specificity"]);
    relative_rdt_sens_->setValue(default_values["relative_RDT_sensitivity"]);
}
