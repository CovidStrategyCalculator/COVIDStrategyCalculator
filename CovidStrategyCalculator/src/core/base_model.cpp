/* base_model.cpp
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
 * This file implements the BaseModel class. The BaseModel class defines the core of the stochastic model.
 */

#include "include/core/base_model.h"
#include <unsupported/Eigen/MatrixFunctions>

const std::vector<int> BaseModel::sub_compartments = {3, 3, 13, 1, 1}; // number of sub compartments per phase
const int BaseModel::n_compartments = 21;                              // sum of sub compartments

// constructor
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
    // no rates are calculated for the risk node
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < sub_compartments[i]; ++j) {
            rates[counter] = sub_compartments[i] / tau_[i];
            counter++;
        }
    }
    this->rates_ = rates;
}

void BaseModel::set_S() {
    // stoichiometric of phases (pre detect, pre sympt, sympt, post sympt)
    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments - 1> S;
    S.fill(0);

    for (int i = 0; i < BaseModel::n_compartments - 1; ++i) {
        S(i, i) = -1;
        S(i + 1, i) = 1;
    }
    this->S_ = S;
}

void BaseModel::set_A() {
    // fill matrix with rates
    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments - 1> rates_matrix;
    rates_matrix.fill(0);
    for (int i = 0; i < BaseModel::n_compartments; ++i) {
        for (int j = 0; j < BaseModel::n_compartments - 1; ++j) {
            rates_matrix(i, j) = rates_[j];
        }
    }

    // coefficient-wise product
    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments - 1> A;
    A = S_.cwiseProduct(rates_matrix);

    Eigen::Matrix<float, BaseModel::n_compartments - 1, BaseModel::n_compartments - 1> A_square;
    A_square = A(Eigen::seq(0, Eigen::last - 1), Eigen::all); // drop last row, sink node not of interest

    // augment A matrix for residual risk calculations: add risk row and column
    Eigen::Matrix<float, BaseModel::n_compartments, BaseModel::n_compartments> A_augmented;
    A_augmented.fill(0);
    A_augmented(Eigen::seq(0, Eigen::last - 1), Eigen::seq(0, Eigen::last - 1)) = A_square;

    int first_infectious_compartment = sub_compartments[0];
    int last_infectious_compartment = sub_compartments[0] + sub_compartments[1] + sub_compartments[2] - 1;
    int first_symptomatic_compartment = sub_compartments[0] + sub_compartments[1];

    A_augmented(Eigen::last, Eigen::seq(first_infectious_compartment, last_infectious_compartment)).array() = 1.;
    // update entry of transition from pre symtomatic phase to symptomatic phase
    A_augmented(first_symptomatic_compartment, first_symptomatic_compartment - 1) *= risk_posing_symptomatic_;

    this->A_ = A_augmented;
}

Eigen::Vector<float, BaseModel::n_compartments> BaseModel::run_base(int time) { return (A_ * (float)time).exp() * X0; }

Eigen::Vector<float, BaseModel::n_compartments>
BaseModel::run_base(int time, Eigen::Vector<float, BaseModel::n_compartments> initial_states) {
    X0 = initial_states;
    return run_base(time);
}
