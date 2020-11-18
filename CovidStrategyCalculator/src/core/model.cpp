#include "include/core/model.h"

Model::Model(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
             Eigen::VectorXf initial_states, int time, std::vector<int> test_indices, float test_sensitivity,
             float test_specificity)
    : BaseModel(residence_times, risk_posing_fraction_symptomatic_phase, initial_states) {
    t_end = time;
    t_test = test_indices;
    sensitivity = test_sensitivity;
    specificity = test_specificity;
    set_false_ommision_rate();
}

Model::Model(std::vector<float> residence_times, Eigen::VectorXf initial_states, int time)
    : Model(residence_times, 1, initial_states, time, {}, .8, .9){};

void Model::set_false_ommision_rate() {
    Eigen::VectorXf FOR(Model::n_compartments);
    FOR.fill(1.);

    int counter = 0;
    for (int j = 0; j < sub_compartments[0]; ++j) {
        FOR[counter] = specificity;
        counter++;
    }
    for (int j = 0; j < sub_compartments[1] + sub_compartments[2]; ++j) {
        FOR[counter] = 1 - sensitivity;
        counter++;
    }

    false_ommision_rate = FOR;
}

Eigen::MatrixXf Model::run_no_test(int time) {
    Eigen::MatrixXf states(Model::n_compartments, time + 1);

    for (int i = 0; i < time + 1; ++i) {
        states.col(i) = this->run_base(i);
    }
    return states;
}

Eigen::MatrixXf Model::run_no_test() {
    int time = t_end;
    Eigen::MatrixXf X_transposed = run_no_test(time);
    X_transposed.transposeInPlace();
    return X_transposed;
}

Eigen::MatrixXf Model::run() {
    int n_eval_states = t_end + t_test.size() + 1;
    Eigen::MatrixXf states(Model::n_compartments, n_eval_states);

    int day_counter = 0;
    int next_idx = 0;
    int t_diff = 0;

    for (int i = 0; i < (int)t_test.size(); ++i) {
        t_diff = t_test[i] - day_counter;
        states(Eigen::all, Eigen::seq(next_idx, next_idx + t_diff)).array() = run_no_test(t_diff).array();
        X0.array() = false_ommision_rate.array() * states(Eigen::all, next_idx + t_diff).array();
        day_counter += t_diff;
        next_idx = next_idx + t_diff + 1;
    }
    t_diff = t_end - day_counter;
    states(Eigen::all, Eigen::seq(next_idx, Eigen::last)).array() = run_no_test(t_diff).array();
    states.transposeInPlace();
    return states;
}

Eigen::VectorXf Model::integrate(Eigen::MatrixXf X) {
    Eigen::VectorXf risk_at_t_inf(X.rows());
    X.transposeInPlace();

    for (int i = 0; i < X.cols(); ++i) {
        risk_at_t_inf[i] = this->run_base(100 - i, X.col(i))(Eigen::last);
    }
    return risk_at_t_inf;
}
