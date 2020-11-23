#include "include/core/base_model.h"
#include <unsupported/Eigen/MatrixFunctions>

// std::vector<int> BaseModel::sub_compartments = {5, 1, 13, 1, 1};
// int BaseModel::n_compartments = 21;

BaseModel::BaseModel(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
                     Eigen::Vector<float, BaseModel::n_compartments> initial_states) {

    tau_ = residence_times;
    risk_posing_symptomatic_ = risk_posing_fraction_symptomatic_phase;
    X0 = initial_states;

    set_rates();
    set_S();
    set_A();
}

void BaseModel::set_rates() {
    Eigen::Vector<float, BaseModel::n_compartments> rates;
    int counter = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < sub_compartments[i]; ++j) {
            rates[counter] = sub_compartments[i] / tau_[i];
            counter++;
        }
    }
    this->rates_ = rates;
}

void BaseModel::set_S() {
    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments - 1> S;
    S.fill(0);

    for (int i = 0; i < BaseModel::n_compartments - 1; ++i) {
        S(i, i) = -1;
        S(i + 1, i) = 1;
    }
    this->S_ = S;
}

void BaseModel::set_A() {
    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments - 1> rates_matrix;
    rates_matrix.fill(0);
    for (int i = 0; i < BaseModel::n_compartments; ++i) {
        for (int j = 0; j < BaseModel::n_compartments - 1; ++j) {
            rates_matrix(i, j) = rates_[j];
        }
    }

    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments - 1> A;
    A = S_.cwiseProduct(rates_matrix);

    Eigen::Matrix<float, BaseModel::n_compartments - 1, BaseModel::n_compartments - 1> A_square;
    A_square = A(Eigen::seq(0, Eigen::last - 1), Eigen::all); // drop last row, sink node not of interest

    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments> A_augmented;
    A_augmented.fill(0);
    A_augmented(Eigen::seq(0, Eigen::last - 1), Eigen::seq(0, Eigen::last - 1)) = A_square;

    int first_infectious_compartment = sub_compartments[0];
    int last_infectious_compartment = sub_compartments[0] + sub_compartments[1] + sub_compartments[2] - 1;
    int first_symptomatic_compartment = sub_compartments[0] + sub_compartments[1];

    A_augmented(Eigen::last, Eigen::seq(first_infectious_compartment, last_infectious_compartment)).array() = 1.;
    A_augmented(first_symptomatic_compartment, first_symptomatic_compartment - 1) *= risk_posing_symptomatic_;

    this->A_ = A_augmented;
}

Eigen::Vector<float, BaseModel::n_compartments> BaseModel::run_base(int time) { return (A_ * (float)time).exp() * X0; }

Eigen::Vector<float, BaseModel::n_compartments>
BaseModel::run_base(int time, Eigen::Vector<float, BaseModel::n_compartments> initial_states) {
    X0 = initial_states;
    return run_base(time);
}
