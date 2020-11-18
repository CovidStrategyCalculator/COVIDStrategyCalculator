#include "include/core/prevalence_estimator.h"

// pre-simulation for prevalence estimator
PrevalenceEstimator::PrevalenceEstimator(ParametersTab *parameters_tab, PrevalenceTab *prevalence_tab)
    : Simulation(parameters_tab) {
    std::vector<float> incidence = prevalence_tab->incidence();

    t_end = 0;
    risk_posing_fraction_symptomatic_phase = 1.;
    expected_adherence = 1.;
    t_test = {};

    estimate_prevalence(incidence);
}

void PrevalenceEstimator::estimate_prevalence(std::vector<float> weekly_incidence) {

    Eigen::MatrixXf X_mean;
    Eigen::MatrixXf X_best;
    Eigen::MatrixXf X_worst;

    Eigen::VectorXf X0;
    X0.setZero(Model::n_compartments);

    model_mean_case_no_intervention = new Model(tau_mean_case, X0, t_end);   // without tests or symptomatic screening
    model_best_case_no_intervention = new Model(tau_best_case, X0, t_end);   // without tests or symptomatic screening
    model_worst_case_no_intervention = new Model(tau_worst_case, X0, t_end); // without tests or symptomatic screening

    Eigen::VectorXf cumulative_states_today_mean;
    Eigen::VectorXf cumulative_states_today_best;
    Eigen::VectorXf cumulative_states_today_worst;

    cumulative_states_today_mean.setZero(Model::n_compartments);
    cumulative_states_today_best.setZero(Model::n_compartments);
    cumulative_states_today_worst.setZero(Model::n_compartments);

    float total_tau = std::accumulate(tau_mean_case.begin(), tau_mean_case.end(), 0);

    for (int week = 0; week < 5; ++week) {
        float day_incidence = weekly_incidence[week] / 7;
        t_end = (week + 1) * 7 - 1;

        // set intial states according to incidence report
        int counter = 0;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < Model::sub_compartments[i]; ++j) {
                float probability = tau_mean_case[i] / total_tau * day_incidence / Model::sub_compartments[i];
                X0(counter) = probability;
                counter++;
            }
        }

        model_mean_case_no_intervention->set_t_end(t_end);
        model_best_case_no_intervention->set_t_end(t_end);
        model_worst_case_no_intervention->set_t_end(t_end);

        model_mean_case_no_intervention->X0 = X0;
        model_best_case_no_intervention->X0 = X0;
        model_worst_case_no_intervention->X0 = X0;

        X_mean = model_mean_case_no_intervention->run_no_test();
        X_best = model_best_case_no_intervention->run_no_test();
        X_worst = model_worst_case_no_intervention->run_no_test();

        cumulative_states_today_mean.array() +=
            X_mean(Eigen::seq(Eigen::last - 6, Eigen::last), Eigen::all).colwise().sum().array();
        cumulative_states_today_best.array() +=
            X_best(Eigen::seq(Eigen::last - 6, Eigen::last), Eigen::all).colwise().sum().array();
        cumulative_states_today_worst.array() +=
            X_worst(Eigen::seq(Eigen::last - 6, Eigen::last), Eigen::all).colwise().sum().array();

        initial_states.fill(0.); // needed to set initial state of sink compartment back to zero
    }

    Eigen::VectorXf probability_per_phase_today_mean = group_by_phase(cumulative_states_today_mean);
    Eigen::VectorXf probability_per_phase_today_best = group_by_phase(cumulative_states_today_best);
    Eigen::VectorXf probability_per_phase_today_worst = group_by_phase(cumulative_states_today_worst);

    Eigen::MatrixXf states_mtrx(Model::n_compartments, 3);
    states_mtrx.col(0) = cumulative_states_today_mean;
    states_mtrx.col(1) = cumulative_states_today_best;
    states_mtrx.col(2) = cumulative_states_today_worst;

    Eigen::MatrixXf probabilities_mtrx(5, 3);
    probabilities_mtrx.col(0) = probability_per_phase_today_mean;
    probabilities_mtrx.col(1) = probability_per_phase_today_best;
    probabilities_mtrx.col(2) = probability_per_phase_today_worst;

    compartment_states_ = states_mtrx;
    phase_probabilities_ = probabilities_mtrx;
}
