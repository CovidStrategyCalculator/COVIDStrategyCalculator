/* base_model.h
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
 * This file defines the BaseModel class. The BaseModel class defines the core of the stochastic model.
 */

#pragma once

#include <Eigen/Dense>
#include <vector>

class BaseModel {

  public:
    static const std::vector<int> sub_compartments; //{3, 3, 13, 1, 1} defined in cpp
    static const int n_compartments;                // 3+3+13+1+1 = 21

    BaseModel() = default; // constructor
    BaseModel(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
              Eigen::Vector<float, 21> initial_states); // constructor
    ~BaseModel() = default;                             // destructor

    Eigen::Vector<float, 21> X0;                 // initial states
    Eigen::Vector<float, 21> run_base(int time); // calculate states at time
    // calculate states at time, for given initial states
    Eigen::Vector<float, 21> run_base(int time, Eigen::Vector<float, 21> initial_states);

  private:
    float risk_posing_symptomatic_{}; // fraction of asymptomatic cases
    std::vector<float> tau_{};        // vector of residence times per compartment

    Eigen::Vector<float, 21> rates_;
    Eigen::Matrix<float, 21, 21 - 1> S_; // stoichiometric matrix
    Eigen::Matrix<float, 21, 21> A_;

    void set_rates();
    void set_S();
    void set_A();
};
