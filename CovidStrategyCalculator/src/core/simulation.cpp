#include "include/core/simulation.h"

Simulation::Simulation(ParametersTab *parameters_tab) { collect_parameters(parameters_tab); }

Simulation::Simulation(ParametersTab *parameters_tab, StrategyTab *strategy_tab, PrevalenceTab *prevalence_tab) {
    collect_parameters(parameters_tab);
    collect_strategy(strategy_tab);
    deduce_combined_parameters();

    if ((mode == 2) && prevalence_tab->use_prevalence_estimation()) {
        initial_states = prevalence_tab->initial_states(); // main simulation incoming travelers

        if (symptomatic_screening) {
            apply_symptomatic_screening_to_initial_states();
        }
    } else {
        set_initial_states(); // main simulation contact management / isolation
    }

    create_different_scenario_models();
    run_risk_calculation();
}

void Simulation::collect_strategy(StrategyTab *strategy_tab) {
    // strategy is 0-indexed, i.e. for a 10-day quarantine we want the states for day 0 (today) up to
    // and including day 10. Hence, t_end = delay + duration of strategy + 1
    t_offset = strategy_tab->time_delay();
    t_end = strategy_tab->time_delay() + strategy_tab->end_of_strategy();
    t_test = strategy_tab->test_moments();
    mode = strategy_tab->mode();
    symptomatic_screening = strategy_tab->use_symptomatic_screening();
    test_type = strategy_tab->test_type();
    expected_adherence = strategy_tab->expected_adherence();
    p_infectious_t0 = strategy_tab->p_infectious_t0();
}

void Simulation::collect_parameters(ParametersTab *parameters_tab) {
    tau_mean_case.push_back(parameters_tab->predetection_mean());
    tau_mean_case.push_back(parameters_tab->presymptomatic_mean());
    tau_mean_case.push_back(parameters_tab->symptomatic_mean());
    tau_mean_case.push_back(parameters_tab->postinfectious());

    tau_best_case.push_back(parameters_tab->predetection_lower());
    tau_best_case.push_back(parameters_tab->presymptomatic_upper());
    tau_best_case.push_back(parameters_tab->symptomatic_upper());
    tau_best_case.push_back(parameters_tab->postinfectious());

    tau_worst_case.push_back(parameters_tab->predetection_upper());
    tau_worst_case.push_back(parameters_tab->presymptomatic_lower());
    tau_worst_case.push_back(parameters_tab->symptomatic_lower());
    tau_worst_case.push_back(parameters_tab->postinfectious());

    fraction_asymptomatic = parameters_tab->fraction_asymptomatic();
    pcr_sens = parameters_tab->pcr_sensitivity();
    rdt_relative_sens = parameters_tab->relative_rdt_sensitivity();
    test_specificity = parameters_tab->pcr_specificity();
}

void Simulation::deduce_combined_parameters() {
    if (symptomatic_screening) {
        risk_posing_fraction_symptomatic_phase = fraction_asymptomatic;
    } else {
        risk_posing_fraction_symptomatic_phase = 1;
    }

    switch (test_type) {
    case 0: // PCR
        test_sensitivity = pcr_sens;
        break;
    case 1: // RDT
        test_sensitivity = pcr_sens * rdt_relative_sens;
        break;
    default:
        break;
    }
}

void Simulation::set_initial_states() {
    Eigen::VectorXf X0(Model::n_compartments);
    X0.fill(0);

    switch (mode) {
    case 0:
        X0(0) = 1.0;
        break;
    case 1: {
        int first_symptomatic_compartment = Model::sub_compartments[0] + Model::sub_compartments[1];
        X0(first_symptomatic_compartment) = 1.0;
        break;
    }
    default:
        break;
    }
    initial_states = X0;
}

void Simulation::apply_symptomatic_screening_to_initial_states() {
    Eigen::VectorXf screening(Model::n_compartments);
    screening.fill(1);

    int first_symptomatic_compartment = Model::sub_compartments[0] + Model::sub_compartments[1];
    int last_symptomatic_compartment =
        Model::sub_compartments[0] + Model::sub_compartments[1] + Model::sub_compartments[2] - 1;
    screening(Eigen::seq(first_symptomatic_compartment, last_symptomatic_compartment)).array() = fraction_asymptomatic;

    initial_states.array() = screening.array() * initial_states.array();
}

void Simulation::create_different_scenario_models() {
    model_mean_case_no_intervention =
        new Model(tau_mean_case, initial_states, t_end); // without tests or symptomatic screening
    model_best_case_no_intervention =
        new Model(tau_best_case, initial_states, t_end); // without tests or symptomatic screening
    model_worst_case_no_intervention =
        new Model(tau_worst_case, initial_states, t_end); // without tests or symptomatic screening

    model_mean_case_NPI = new Model(tau_mean_case, risk_posing_fraction_symptomatic_phase, initial_states, t_end,
                                    t_test, test_sensitivity, test_specificity);
    model_best_case_NPI = new Model(tau_best_case, risk_posing_fraction_symptomatic_phase, initial_states, t_end,
                                    t_test, test_sensitivity, test_specificity);
    model_worst_case_NPI = new Model(tau_worst_case, risk_posing_fraction_symptomatic_phase, initial_states, t_end,
                                     t_test, test_sensitivity, test_specificity);

    // states per evaluation time
    strategy_states_mean = model_mean_case_NPI->run();
    strategy_states_best = model_best_case_NPI->run();
    strategy_states_worst = model_worst_case_NPI->run();
}

Eigen::MatrixXf Simulation::temporal_assay_sensitivity() {
    Eigen::MatrixXf daily_probability_per_phase_mean = group_by_phase(model_mean_case_no_intervention->run());
    Eigen::MatrixXf daily_probability_per_phase_best = group_by_phase(model_best_case_no_intervention->run());
    Eigen::MatrixXf daily_probability_per_phase_worst = group_by_phase(model_worst_case_no_intervention->run());

    float initial_population = daily_probability_per_phase_mean(0, Eigen::seq(0, 3)).sum(); // needed for scaling
                                                                                            // for incoming travelers

    Eigen::VectorXf p_detectable_mean, p_detectable_best, p_detectable_worst;
    p_detectable_mean =
        (1 - test_specificity) * daily_probability_per_phase_mean(Eigen::all, 0) +
        test_sensitivity * daily_probability_per_phase_mean(Eigen::all, Eigen::seq(1, 3)).rowwise().sum();
    p_detectable_best =
        (1 - test_specificity) * daily_probability_per_phase_best(Eigen::all, 0) +
        test_sensitivity * daily_probability_per_phase_best(Eigen::all, Eigen::seq(1, 3)).rowwise().sum();
    p_detectable_worst =
        (1 - test_specificity) * daily_probability_per_phase_worst(Eigen::all, 0) +
        test_sensitivity * daily_probability_per_phase_worst(Eigen::all, Eigen::seq(1, 3)).rowwise().sum();

    Eigen::MatrixXf p_detectable(t_end + 1, 3);
    p_detectable.col(0) = p_detectable_mean.array() / initial_population;
    p_detectable.col(1) = p_detectable_best.array() / initial_population;
    p_detectable.col(2) = p_detectable_worst.array() / initial_population;

    return p_detectable;
}

Eigen::MatrixXf Simulation::test_efficacy() {
    Eigen::MatrixXf daily_probability_per_phase_mean = group_by_phase(model_mean_case_no_intervention->run());
    Eigen::MatrixXf daily_probability_per_phase_best = group_by_phase(model_best_case_no_intervention->run());
    Eigen::MatrixXf daily_probability_per_phase_worst = group_by_phase(model_worst_case_no_intervention->run());

    Eigen::VectorXf infectious_mean = daily_probability_per_phase_mean(Eigen::all, Eigen::seq(1, 2)).rowwise().sum();
    Eigen::VectorXf infectious_best = daily_probability_per_phase_best(Eigen::all, Eigen::seq(1, 2)).rowwise().sum();
    Eigen::VectorXf infectious_worst = daily_probability_per_phase_worst(Eigen::all, Eigen::seq(1, 2)).rowwise().sum();

    float initial_population = daily_probability_per_phase_mean(0, Eigen::seq(0, 3)).sum(); // needed for scaling
                                                                                            // for incoming travelers

    Eigen::MatrixXf infectious(t_end + 1, 3);
    infectious.col(0) = infectious_mean.array() / initial_population;
    infectious.col(1) = infectious_best.array() / initial_population;
    infectious.col(2) = infectious_worst.array() / initial_population;

    return (infectious * test_sensitivity).array() / temporal_assay_sensitivity().array();
}

void Simulation::run_risk_calculation() {
    risk_no_intervention();
    risk_NPI();
}

void Simulation::risk_no_intervention() {
    Eigen::VectorXf risk_no_intervention_mean;
    Eigen::VectorXf risk_no_intervention_best;
    Eigen::VectorXf risk_no_intervention_worst;

    int n_eval = t_end + t_test.size() + 1;
    Eigen::MatrixXf X0_proxy = initial_states;
    X0_proxy.resize(1, Model::n_compartments);

    // initialize risk_no_intervention vectors
    risk_no_intervention_mean.setZero(n_eval, 1);
    risk_no_intervention_best.setZero(n_eval, 1);
    risk_no_intervention_worst.setZero(n_eval, 1);

    float tmp; // helper variable to temporarily store result
    tmp = model_mean_case_no_intervention->integrate(X0_proxy)(0) - X0_proxy(0, Eigen::last);
    risk_no_intervention_mean.fill(tmp);
    tmp = model_best_case_no_intervention->integrate(X0_proxy)(0) - X0_proxy(0, Eigen::last);
    risk_no_intervention_best.fill(tmp);
    tmp = model_worst_case_no_intervention->integrate(X0_proxy)(0) - X0_proxy(0, Eigen::last);
    risk_no_intervention_worst.fill(tmp);

    Eigen::MatrixXf risk(n_eval, 3);
    risk.col(0) = risk_no_intervention_mean;
    risk.col(1) = risk_no_intervention_best;
    risk.col(2) = risk_no_intervention_worst;

    risk_matrix_no_intervention = risk;
}

void Simulation::risk_NPI() {

    Eigen::VectorXf risk_NPI_mean;
    Eigen::VectorXf risk_NPI_best;
    Eigen::VectorXf risk_NPI_worst;

    risk_NPI_mean =
        model_mean_case_NPI->integrate(strategy_states_mean) - strategy_states_mean(Eigen::all, Eigen::last);
    risk_NPI_best =
        model_best_case_NPI->integrate(strategy_states_best) - strategy_states_best(Eigen::all, Eigen::last);
    risk_NPI_worst =
        model_worst_case_NPI->integrate(strategy_states_worst) - strategy_states_worst(Eigen::all, Eigen::last);

    Eigen::MatrixXf risk(risk_NPI_mean.size(), 3);

    risk.col(0) =
        (expected_adherence * risk_NPI_mean + (1 - expected_adherence) * risk_matrix_no_intervention.col(0)).array();
    risk.col(1) =
        (expected_adherence * risk_NPI_best + (1 - expected_adherence) * risk_matrix_no_intervention.col(1)).array();
    risk.col(2) =
        (expected_adherence * risk_NPI_worst + (1 - expected_adherence) * risk_matrix_no_intervention.col(2)).array();

    risk_matrix_NPI = risk;
}

Eigen::MatrixXf Simulation::relative_risk() {
    return (expected_adherence * risk_matrix_NPI + (1. - expected_adherence) * risk_matrix_no_intervention).array() /
           risk_matrix_no_intervention.array();
}

Eigen::MatrixXf Simulation::risk_reduction() {
    Eigen::MatrixXf M(risk_matrix_no_intervention.rows(), 3);
    M.fill(1);
    return M.array() - relative_risk().array();
}

Eigen::MatrixXf Simulation::fold_risk_reduction() {
    return risk_matrix_no_intervention.array() /
           (expected_adherence * risk_matrix_NPI + (1. - expected_adherence) * risk_matrix_no_intervention).array();
}

Eigen::MatrixXf Simulation::group_by_phase(Eigen::MatrixXf states) {

    int n_eval = states.rows();

    int transpose = false;
    if (states.cols() == 1) { // states is a column vector
        states.transposeInPlace();
        n_eval = 1;
        transpose = true;
    }

    Eigen::MatrixXf grouped(n_eval, 5);
    int counter = 0;
    for (int i = 0; i < 5; ++i) {
        grouped(Eigen::all, i) =
            states(Eigen::all, Eigen::seq(counter, counter + Model::sub_compartments.at(i) - 1)).rowwise().sum();
        counter = counter + Model::sub_compartments.at(i);
    }

    if (transpose) { // if states was a column vector, return as column vector
        grouped.transposeInPlace();
    }
    return grouped;
}

Eigen::VectorXf Simulation::evaluation_points_with_tests() {
    int n_eval_points = t_end + t_test.size() + 1;
    Eigen::VectorXf evaluation_points(n_eval_points);

    int time_counter = -t_offset;
    int index_counter = 0;
    int t_diff = 0;
    for (int i = 0; i < (int)t_test.size(); ++i) {
        t_diff = (t_test[i] - t_offset) - time_counter;
        for (int day = 0; day < t_diff + 1; ++day) {
            evaluation_points(index_counter) = time_counter + day;
            ++index_counter;
        }
        time_counter += t_diff;
    }

    t_diff = (t_end - t_offset) - time_counter;
    for (int day = 0; day < t_diff + 1; ++day) {
        evaluation_points(index_counter) = time_counter + day;
        ++index_counter;
    }

    return evaluation_points;
}

Eigen::VectorXf Simulation::evaluation_points_without_tests() {
    int n_eval_points = t_end + 1;
    Eigen::VectorXf evaluation_points(n_eval_points);

    for (int idx = 0; idx < n_eval_points; ++idx) {
        evaluation_points(idx) = -t_offset + idx;
    }
    return evaluation_points;
}

//     // TODO only used in prevalence estimator?
//     // daily probability; needed for sensitivity assay
//     daily_probability_per_phase_mean = group_by_phase(model_mean_case_NPI->run_no_test());
//     daily_probability_per_phase_best = group_by_phase(model_best_case_NPI->run_no_test());
//     daily_probability_per_phase_worst = group_by_phase(model_worst_case_NPI->run_no_test());
// }
