/* prevalence_estimator.h
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
 * This file defines the PrevalenceEstimator class, which derives from, and extends the Simulation class.
 * The objective of the PrevalenceEstimator class is to execute a single simulation to generate a population with
 * mixed ‘infection age’ for the analysis of NPI strategies.
 */

#pragma once

#include "include/core/simulation.h"

#include <Eigen/Dense>
#include <vector>

class PrevalenceEstimator : public Simulation {

  private:
    // runs prevalence estimation based on incidence reports
    void estimate_prevalence(std::vector<float> weekly_incidence);

    Eigen::MatrixXf compartment_states_;  // probability per compartment
    Eigen::MatrixXf phase_probabilities_; // probability per phase

  public:
    PrevalenceEstimator() = default; // constructor
    explicit PrevalenceEstimator(ParametersTab *parameters_tab,
                                 PrevalenceTab *prevalence_tab); // pre-simulation for prevalence estimator
    ~PrevalenceEstimator() = default;                            // destructor

    // getter functions
    Eigen::MatrixXf compartment_states() { return compartment_states_; }
    Eigen::MatrixXf phase_probabilities() { return phase_probabilities_; }
};
