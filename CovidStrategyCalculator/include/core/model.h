/* model.h
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
 * This file defines the Model class, which derives from-, and extends the BaseModel class.
 * The Model class extends the BaseModel for diagnostic testing and calculation of the residual transmission risk that
 * remains after the NPI strategy.
 */

#pragma once

#include "include/core/base_model.h"

#include <Eigen/Dense>
#include <vector>

class Model : public BaseModel {

    int t_end{};               // time point marking end of NPI
    std::vector<int> t_test{}; // time points at which to perform a diagnostic test

    int test_type;                       // PCR=0, RDT=1
    float specificity{};                 // specificity of diagnostic test
    float sensitivity{};                 // sensitivity of diagnostic test
    Eigen::VectorXf false_ommision_rate; // compartment dependent false ommision rates
    void set_false_ommision_rate();
    void set_false_ommision_rate_PCR();
    void set_false_ommision_rate_RDT();

    Eigen::MatrixXf run_no_test(int time); // run the model without tests

  public:
    Model() = default; // constructor
    Model(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
          Eigen::VectorXf initial_states, int time, std::vector<int> test_indices, int test_type,
          float test_sensitivity,
          float test_specificity); // constructor
    ~Model() = default;            // destructor

    // no test or symptomatic screening
    Model(std::vector<float> residence_times, Eigen::VectorXf initial_states, int time);

    Eigen::MatrixXf run();
    Eigen::MatrixXf run_no_test();
    Eigen::VectorXf integrate(Eigen::MatrixXf X); // calculation of the residual transmission risk
    void set_t_end(int new_t_end) { t_end = new_t_end; }
};
