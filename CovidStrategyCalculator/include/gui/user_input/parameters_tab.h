/* parameters_tab.h
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
 * This file defines the ParametersTab class for the parameter-related user input which derives from QWidget.
 * The ParametersTab provides the second tab of the user input field and bundles together the model parameters of the
 * simulation. The parameters in this class are independent of the user-defined NPI strategy.
 */

#pragma once

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QWidget>

class ParametersTab : public QWidget {
    Q_OBJECT

  private:
    // private member variables corresponding to the input fields / buttons in the widget
    QDoubleSpinBox *incubation_mean_;
    QDoubleSpinBox *incubation_upper_;
    QDoubleSpinBox *incubation_lower_;
    QDoubleSpinBox *symptomatic_mean_;
    QDoubleSpinBox *symptomatic_upper_;
    QDoubleSpinBox *symptomatic_lower_;
    QDoubleSpinBox *postinfectious_;
    QDoubleSpinBox *percentage_predetection_;
    QDoubleSpinBox *percentage_asymptomatic_;
    QDoubleSpinBox *pcr_sens_;
    QDoubleSpinBox *pcr_spec_;
    QDoubleSpinBox *relative_rdt_sens_;
    QPushButton *reset_button_;

    // default values for the parameters, used to initialize and reset the fields to their default values
    std::map<std::string, float> default_values{{"duration_incubation_lower", 5.60},
                                                {"duration_incubation_mean", 6.77},
                                                {"duration_incubation_upper", 7.99},
                                                {"percentage_of_incubation_predetection", 42.2},
                                                {"duration_symptomatic_lower", 2.79},
                                                {"duration_symptomatic_mean", 7.50},
                                                {"duration_symptomatic_upper", 11.47},
                                                {"percentage_asymptomatic", 20},
                                                {"duration_postinfectious_mean", 8},
                                                {"PCR_sensitivity", 80},
                                                {"PCR_specificity", 99.99},
                                                {"relative_RDT_sensitivity", 85}};

    // functions to initialize the widget
    void initialize_member_variables();
    void set_layout();
    // resets all input fields to their default values; to be executed when the 'reset defaults' button is clicked
    void reset_defaults();

  public:
    explicit ParametersTab(QWidget *parent = nullptr);

    /* Read-only getter functions.
     *
     * All user input values provided as percentual values are first scaled to probabilities between 0 and 1.
     *
     * The mean duration of the incubation period and the percentage thereof that is predetectable are both user
     * defined. These two parameters are used to determine the mean duration of the predection and presymptomatic
     * phases. Where the predetection phase = (percentage / 100) * incubation, and presymptomatic =
     * (1 - percentage/100) * incubation.
     */
    float predetection_mean() const { return percentage_predetection_->value() / 100 * incubation_mean_->value(); }
    float predetection_upper() const { return percentage_predetection_->value() / 100 * incubation_upper_->value(); }
    float predetection_lower() const { return percentage_predetection_->value() / 100 * incubation_lower_->value(); }
    float presymptomatic_mean() const {
        return (1 - percentage_predetection_->value() / 100) * incubation_mean_->value();
    }
    float presymptomatic_upper() const {
        return (1 - percentage_predetection_->value() / 100) * incubation_upper_->value();
    }
    float presymptomatic_lower() const {
        return (1 - percentage_predetection_->value() / 100) * incubation_lower_->value();
    }
    float symptomatic_mean() const { return symptomatic_mean_->value(); }
    float symptomatic_upper() const { return symptomatic_upper_->value(); }
    float symptomatic_lower() const { return symptomatic_lower_->value(); }
    float postinfectious() const { return postinfectious_->value(); }
    float fraction_asymptomatic() const { return percentage_asymptomatic_->value() / 100.; } // percent to probability
    float pcr_sensitivity() const { return pcr_sens_->value() / 100.; }                      // percent to probability
    float pcr_specificity() const { return pcr_spec_->value() / 100.; }                      // percent to probability
    float relative_rdt_sensitivity() const { return relative_rdt_sens_->value() / 100.; }    // percent to probability
};
