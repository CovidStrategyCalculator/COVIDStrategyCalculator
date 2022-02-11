/* simulation.h
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
 * This file defines the Simulation class.
 * The objective of the Simulation class is to execute and calculate the efficaty of an arbitrary NPI strategy.
 * The Simulation handles the comparison of the strategy, to the baseline case.
 */

#pragma once

#include "include/core/model.h"
#include "include/gui/user_input/parameters_tab.h"
#include "include/gui/user_input/prevalence_tab.h"
#include "include/gui/user_input/strategy_tab.h"

#include <Eigen/Dense>

class Simulation {

  public:
    Simulation() = default;                             // constructor
    explicit Simulation(ParametersTab *parameters_tab); // constructor
    explicit Simulation(ParametersTab *parameters_tab, StrategyTab *strategy,
                        PrevalenceTab *prevalence_tab); // constructor
    ~Simulation() = default;                            // destructor

    // calculate efficacy of current strategy
    Eigen::MatrixXf relative_risk();
    Eigen::MatrixXf risk_reduction();
    Eigen::MatrixXf fold_risk_reduction();

    Eigen::VectorXf evaluation_points_with_tests();    // time course with the defined tests
    Eigen::VectorXf evaluation_points_without_tests(); // time course without conducting defined tests

    Eigen::MatrixXf temporal_assay_sensitivity(); // for plot
    Eigen::MatrixXf temporal_assay_sensitivity_PCR();
    Eigen::MatrixXf temporal_assay_sensitivity_RDT(); // for plot
    Eigen::MatrixXf test_efficacy();                  // for efficacy table
    Eigen::MatrixXf test_efficacy_PCR();
    Eigen::MatrixXf test_efficacy_RDT();

    // getter functions
    int get_mode() { return mode; }
    bool get_symptomatic_screening() { return symptomatic_screening; }
    float get_fraction_asymptomatic() { return fraction_asymptomatic; }
    float get_adherence() { return expected_adherence; }
    int get_t_offset() { return t_offset; }
    int get_last_t() { return t_end - t_offset; } // time between last test and end of NPI
    int get_test_type() { return test_type; }
    float get_p_infectious_t0() { return p_infectious_t0; }
    Eigen::VectorXf get_p_infectious_tend();
    std::vector<int> get_t_test() { return t_test; };

  protected:
    // initialization
    void collect_parameters(ParametersTab *parameters_tab);
    void collect_strategy(StrategyTab *strategy_tab);
    void deduce_combined_parameters();
    Eigen::VectorXf initial_states_no_intervention;
    Eigen::VectorXf initial_states_NPI;
    void set_initial_states();
    void apply_symptomatic_screening_to_initial_states();

    // the different models that are needed to calculate relative and extreme values
    void create_different_scenario_models();
    Model *model_mean_case_no_intervention;
    Model *model_best_case_no_intervention;
    Model *model_worst_case_no_intervention;

    Model *model_mean_case_NPI;
    Model *model_best_case_NPI;
    Model *model_worst_case_NPI;

    // the compartment states of the different models
    Eigen::MatrixXf strategy_states_mean;
    Eigen::MatrixXf strategy_states_best;
    Eigen::MatrixXf strategy_states_worst;

    Eigen::MatrixXf states_mean_no_intervention;
    Eigen::MatrixXf states_best_no_intervention;
    Eigen::MatrixXf states_worst_no_intervention;

    // parameters from strategy_tab
    std::vector<int> t_test{};
    int t_offset; // time delay
    int t_end;    // time delay + duration of strategy
    int mode;
    int test_type;
    float expected_adherence;
    float p_infectious_t0;      // the initial probability of infection
    bool symptomatic_screening; // indicator variable whether symptom screening is to be used

    // parameters from parameters_tab
    std::vector<float> tau_mean_case{};  // residence times in typical case
    std::vector<float> tau_best_case{};  // residence times in extreme case
    std::vector<float> tau_worst_case{}; // residence times in extreme case
    float fraction_asymptomatic;         // probability of an asymptomatic disease course
    float pcr_sens;                      // maximal clinical sensitivity of PCR test
    float rdt_relative_sens;             // sensitivity of RDT relative to PCR test
    float test_specificity;              // test specificity of PCR and RDT are assumed to be similar.

    // parameters from strategy_tab + parameters_tab
    float risk_posing_fraction_symptomatic_phase;
    float test_sensitivity;

    // risk calculations
    void run_risk_calculation();
    void risk_no_intervention();
    void risk_NPI();
    Eigen::MatrixXf risk_matrix_no_intervention;
    Eigen::MatrixXf risk_matrix_NPI;

    // matrices grouped by phase are used in the prevalence estimator and daily probabilities
    Eigen::MatrixXf group_by_phase(Eigen::MatrixXf states);
    Eigen::MatrixXf group_by_phase_RDT(Eigen::MatrixXf states);
};
