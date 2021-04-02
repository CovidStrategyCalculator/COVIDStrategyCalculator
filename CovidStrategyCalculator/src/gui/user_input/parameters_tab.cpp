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
    char tt_percentage_predetection_[] = "<html><head/><body><p> "
                                         "Duration incubation period = duration predetectable period "
                                         "+ duration presymptomatic, detectable period. "
                                         "The value in this field indicates the ratio between the duration "
                                         "of the predetectable period and the total incubation period."
                                         "</p></body></html>";
    percentage_predetection_->setToolTip(tt_percentage_predetection_);
    symptomatic_mean_ = Utils::create_DoubleSpinBox(default_values["duration_symptomatic_mean"], 0.01, 100, 2);
    symptomatic_upper_ = Utils::create_DoubleSpinBox(default_values["duration_symptomatic_upper"], 0.01, 100, 2);
    symptomatic_lower_ = Utils::create_DoubleSpinBox(default_values["duration_symptomatic_lower"], 0.01, 100, 2);
    percentage_asymptomatic_ = Utils::create_DoubleSpinBox(default_values["percentage_asymptomatic"], 0.01, 100, 2);
    postinfectious_ = Utils::create_DoubleSpinBox(default_values["duration_postinfectious_mean"], 0.01, 100, 2);
    pcr_sens_ = Utils::create_DoubleSpinBox(default_values["PCR_sensitivity"], 0.01, 100, 2);
    pcr_spec_ = Utils::create_DoubleSpinBox(default_values["PCR_specificity"], 0.01, 100, 2);
    relative_rdt_sens_ = Utils::create_DoubleSpinBox(default_values["relative_RDT_sensitivity"], 0.01, 100, 2);
    char tt_relative_rdt_sens_[] =
        "<html><head/><body><p> "
        "Effective sensitivity antigen-test = relative sensitivity to PCR * PCR sensitivity. "
        "</p></body></html>";
    relative_rdt_sens_->setToolTip(tt_relative_rdt_sens_);

    reset_button_ = new QPushButton(tr("Reset defaults"));
    reset_button_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(reset_button_, &QPushButton::clicked, [=]() { reset_defaults(); });
}

void ParametersTab::set_layout() {
    QGridLayout *main_layout = new QGridLayout;
    main_layout->setAlignment(Qt::AlignCenter);

    QLabel *label_lower = new QLabel(tr("Lower extreme value"));
    char tt_label_lower[] = "<html><head/><body><p> "
                            "Extreme case dynamics low, average value. "
                            "</p></body></html>";
    label_lower->setToolTip(tt_label_lower);
    main_layout->addWidget(label_lower, 0, 1, Qt::AlignCenter);

    QLabel *label_typical = new QLabel(tr("Typical"));
    char tt_label_typical[] = "<html><head/><body><p> "
                              "Typical case dynamics, average value. "
                              "</p></body></html>";
    label_typical->setToolTip(tt_label_typical);
    main_layout->addWidget(label_typical, 0, 2, Qt::AlignCenter);

    QLabel *label_upper = new QLabel(tr("Upper extreme value"));
    char tt_label_upper[] = "<html><head/><body><p> "
                            "Extreme case dynamics high, average value. "
                            "</p></body></html>";
    label_upper->setToolTip(tt_label_upper);
    main_layout->addWidget(label_upper, 0, 3, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Mean duration of incubation period [days]:")), 1, 0);
    main_layout->addWidget(incubation_mean_, 1, 2, Qt::AlignCenter);
    main_layout->addWidget(incubation_upper_, 1, 3, Qt::AlignCenter);
    main_layout->addWidget(incubation_lower_, 1, 1, Qt::AlignCenter);

    QLabel *label_predetect = new QLabel(tr("Percentage thereof pre-detectable [%]:"));
    char tt_label_predetect[] = "<html><head/><body><p> "
                                "Duration incubation period = duration predetectable period "
                                "+ duration presymptomatic, detectable period. "
                                "The value in this field indicates the ratio between the duration "
                                "of the predetectable period and the total incubation period."
                                "</p></body></html>";
    label_predetect->setToolTip(tt_label_predetect);
    main_layout->addWidget(label_predetect, 2, 0);
    main_layout->addWidget(percentage_predetection_, 2, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Mean duration of symptomatic period [days]:")), 3, 0);
    main_layout->addWidget(symptomatic_mean_, 3, 2, Qt::AlignCenter);
    main_layout->addWidget(symptomatic_upper_, 3, 3, Qt::AlignCenter);
    main_layout->addWidget(symptomatic_lower_, 3, 1, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("Mean duration of post-symptomatic, detectable period [days]:")), 4, 0);
    main_layout->addWidget(postinfectious_, 4, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("PCR-test sensitivity [%]")), 5, 0);
    main_layout->addWidget(pcr_sens_, 5, 2, Qt::AlignCenter);

    main_layout->addWidget(new QLabel(tr("PCR-test specificity [%]")), 6, 0);
    main_layout->addWidget(pcr_spec_, 6, 2, Qt::AlignCenter);

    QLabel *label_relative_rdt_sens = new QLabel(tr("Antigen-test sensitivity, relative to PCR-test [%]"));
    char tt_label_relative_rdt_sens[] =
        "<html><head/><body><p> "
        "Effective sensitivity antigen-test = relative sensitivity to PCR * PCR sensitivity. "
        "</p></body></html>";
    label_relative_rdt_sens->setToolTip(tt_label_relative_rdt_sens);
    main_layout->addWidget(label_relative_rdt_sens, 7, 0);
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
