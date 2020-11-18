#pragma once

#include "include/core/model.h"
#include "include/gui/user_input/parameters_tab.h"
#include "include/gui/user_input/prevalence_tab.h"
#include "include/gui/user_input/strategy_tab.h"

#include <Eigen/Dense>

class Simulation {

  public:
    Simulation() = default;
    explicit Simulation(ParametersTab *parameters_tab);
    explicit Simulation(ParametersTab *parameters_tab, StrategyTab *strategy, PrevalenceTab *prevalence_tab);
    ~Simulation() = default;

    Eigen::MatrixXf relative_risk();
    Eigen::MatrixXf risk_reduction();
    Eigen::MatrixXf fold_risk_reduction();

    Eigen::VectorXf evaluation_points_with_tests();
    Eigen::VectorXf evaluation_points_without_tests();

    Eigen::MatrixXf temporal_assay_sensitivity();
    Eigen::MatrixXf test_efficacy();

    int get_mode() { return mode; }
    bool get_symptomatic_screening() { return symptomatic_screening; }
    float get_fraction_asymptomatic() { return fraction_asymptomatic; }
    float get_adherence() { return expected_adherence; }
    int get_t_offset() { return t_offset; }
    int get_last_t() { return t_end - t_offset; }
    int get_test_type() { return test_type; }
    float get_p_infectious_t0() { return p_infectious_t0; }
    Eigen::VectorXf get_p_infectious_tend();
    std::vector<int> get_t_test() { return t_test; };

  protected:
    // initialization
    void collect_parameters(ParametersTab *parameters_tab);
    void collect_strategy(StrategyTab *strategy_tab);
    void deduce_combined_parameters();
    Eigen::Vector<float, Model::n_compartments> initial_states_no_intervention;
    Eigen::Vector<float, Model::n_compartments> initial_states_NPI;
    void set_initial_states();
    void apply_symptomatic_screening_to_initial_states();

    // the different models
    void create_different_scenario_models();
    Model *model_mean_case_no_intervention;
    Model *model_best_case_no_intervention;
    Model *model_worst_case_no_intervention;

    Model *model_mean_case_NPI;
    Model *model_best_case_NPI;
    Model *model_worst_case_NPI;

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
    float p_infectious_t0;
    bool symptomatic_screening;

    // parameters from parameters_tab
    std::vector<float> tau_mean_case{};
    std::vector<float> tau_best_case{};
    std::vector<float> tau_worst_case{};
    float fraction_asymptomatic; // probability of an asymptomatic disease course
    float pcr_sens;
    float rdt_relative_sens;
    float test_specificity;

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
};
