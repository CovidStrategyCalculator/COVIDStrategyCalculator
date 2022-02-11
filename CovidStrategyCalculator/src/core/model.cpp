/* model.cpp
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
 * This file implements the Model class, which derives from-, and extends the BaseModel class.
 * The Model class extends the BaseModel for diagnostic testing and calculation of the residual transmission risk that
 * remains after the NPI strategy.
 */

#include "include/core/model.h"

Model::Model(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
             Eigen::VectorXf initial_states, int time, std::vector<int> test_indices, int type_of_test,
             float test_sensitivity, float test_specificity)
    : BaseModel(residence_times, risk_posing_fraction_symptomatic_phase, initial_states) {
    t_end = time;
    t_test = test_indices;
    test_type = type_of_test;
    sensitivity = test_sensitivity;
    specificity = test_specificity;
    set_false_ommision_rate();
}

Model::Model(std::vector<float> residence_times, Eigen::VectorXf initial_states, int time)
    : Model(residence_times, 1, initial_states, time, {}, 0, .8, .999){};

void Model::set_false_ommision_rate() {
    if (this->test_type == 0) {
        set_false_ommision_rate_PCR();
    } else
        set_false_ommision_rate_RDT();
}

void Model::set_false_ommision_rate_PCR() {
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

void Model::set_false_ommision_rate_RDT() {
    Eigen::VectorXf FOR(Model::n_compartments);
    FOR.fill(1.);

    int counter = 0;
    for (int j = 0; j < sub_compartments[0] + 2; ++j) {
        FOR[counter] = specificity;
        counter++;
    }
    for (int j = 0; j < sub_compartments[1] - 2 + sub_compartments[2] - 5; ++j) {
        FOR[counter] = 1 - sensitivity;
        counter++;
    }
    for (int j = 0; j < 5; ++j) {
        FOR[counter] = specificity;
        counter++;
    }

    false_ommision_rate = FOR;
}

Eigen::MatrixXf Model::run_no_test(int time) {
    Eigen::MatrixXf states(Model::n_compartments, time + 1); // +1 because of start at day=0

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

// The model is executed in 1-day steps to obtain all the points required for plotting
Eigen::MatrixXf Model::run() {
    int n_eval_states = t_end + t_test.size() + 1; // +1 because strategy is 0-indexed
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

// calculates the residual risk
Eigen::VectorXf Model::integrate(Eigen::MatrixXf X) {
    Eigen::VectorXf risk_at_t_inf(X.rows());
    X.transposeInPlace();

    for (int i = 0; i < X.cols(); ++i) {
        risk_at_t_inf[i] = this->run_base(100 - i, X.col(i))(Eigen::last); // t_inf=100
    }
    return risk_at_t_inf;
}
