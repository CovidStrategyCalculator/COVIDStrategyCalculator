/* prevalence_tab.h
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
 * This file defines the PrevalenceTab class for the prevalence-related user input which derives from QWidget.
 * The PrevalenceTab provides the third tab of the user input field and acts as the interface to the prevalence
 * estimator feature of the COVIDStrategycalculator.
 * This widget allows users to run a prevalence estimation based on the recent incidence reports of a region/country and
 * to use the results (a mix of ‘infection ages’) as the initital states of the main simulation.
 */

#pragma once

#include "include/gui/user_input/parameters_tab.h"

#include <Eigen/Dense>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class PrevalenceTab : public QWidget {
    Q_OBJECT

  private:
    // user input fields
    QSpinBox *reports4_;
    QSpinBox *reports3_;
    QSpinBox *reports2_;
    QSpinBox *reports1_;
    QSpinBox *reports0_;
    QDoubleSpinBox *percent_of_cases_detected_;

    // dynamically updated labels to display raw input as percentual incidence
    QLabel *percent_of_population4_;
    QLabel *percent_of_population3_;
    QLabel *percent_of_population2_;
    QLabel *percent_of_population1_;
    QLabel *percent_of_population0_;

    QPushButton *run_button_;

    // members to display the result of the prevalence estimation
    QGroupBox *result_box_;
    // results of estimation using 'typical' case parameters
    QLabel *result_predetect_;
    QLabel *result_presympt_;
    QLabel *result_sympt_;
    QLabel *result_postinf_;
    QLabel *result_total_prevalence_;
    QLabel *result_inf_prevalence_;
    // results of estimation using best and worst case parameters
    QLabel *result_predetect_range_;
    QLabel *result_presympt_range_;
    QLabel *result_sympt_range_;
    QLabel *result_postinf_range_;
    QLabel *result_total_prevalence_range_;
    QLabel *result_inf_prevalence_range_;

    // variables to store the results of the prevalence estimation, used to initialize the main simulation when selected
    Eigen::MatrixXf compartment_states_;  // shape: n_compartment x 3 (typical, lower estimate, upper estimate)
    Eigen::MatrixXf phase_probabilities_; // shape: 5x3 (predetect, presympt, sympt, postinf, risk node)x(typical, lower
                                          // estimate , upper estimate)

    /* When a prevalence estiamtion is run, the results can be used as initial states for the main (strategy)
     * simulation. To select this option, the user has to select one of the checkboxes in the results (typical case,
     * best case, worst case). The variable `use_for_main_simulation_` stores these checkboxes and is used to keep track
     * of their states. The states of these checkboxes is used to configure other elements of the user input container,
     * such as the state of the run button, which, for example, should be disabled in 'incoming travelers' mode when
     * none of these checkboxes is checked.
     */
    std::vector<QCheckBox *> use_for_main_simulation_;
    /* the index of the checked checkbox, indicating which estimation is selected by the user to use in the main
     * simulation. -1 before initialization by the user.
     */
    int use_case{-1};
    bool use_prevalence_estimation_{false}; // indicator variable for whether one of the use_case boxes is checked
    void state_of_use_checkbox_changed();   // defines the actions for when the state of one of the checkboxes in
                                            // `use_for_main_simulation_` changes

    // functions to initialize the widget
    void initialize_member_variables();
    void set_layout();
    // initialize and run an instance of the PrevalenceEstimator based on `this`
    void estimate_prevalence();

  public:
    explicit PrevalenceTab(QWidget *parent = nullptr);

    // read-only getter functions

    /* The probability to be -or to have yet to become infectious at day 0 according to the prevalence estimation. The
     * sum of the probabilites of the predetection, presymptomatic and symptomatic phase. Note that the
     * Eigen::seq(begin, end) functionalality provides the range [begin, end] (both inclusive).
     */
    float p_infectious_t0() const { return phase_probabilities_.col(use_case)(Eigen::seq(0, 2)).sum(); }
    Eigen::VectorXf initial_states() const {
        return compartment_states_.col(use_case);
    } // returns initial state for each compartment
    std::vector<float> incidence();
    bool use_prevalence_estimation() {
        return (use_for_main_simulation_[0]->isChecked() || use_for_main_simulation_[1]->isChecked()) ||
               use_for_main_simulation_[2]->isChecked();
    }

    // setters
    void set_states(Eigen::MatrixXf matrix) { compartment_states_ = matrix; }
    void set_probabilities(Eigen::MatrixXf matrix) { phase_probabilities_ = matrix; }

    // functions to communicate with the InputContainer
    void uncheck_all_use_case_boxes();
    void update_layout();

  signals:
    void use_checkbox_checked();
    void all_boxes_are_unchecked();
    void disable_main_run_button();
    void enable_main_run_button();
    void run_prevalence_estimator();
};
