#pragma once

#include "include/core/simulation.h"

#include <Eigen/Dense>
#include <vector>

class PrevalenceEstimator : public Simulation {

  private:
    void estimate_prevalence(std::vector<float> weekly_incidence);

    Eigen::MatrixXf compartment_states_;
    Eigen::MatrixXf phase_probabilities_;

  public:
    explicit PrevalenceEstimator(ParametersTab *parameters_tab,
                                 PrevalenceTab *prevalence_tab); // pre-simulation for prevalence estimator

    Eigen::MatrixXf compartment_states() { return compartment_states_; }
    Eigen::MatrixXf phase_probabilities() { return phase_probabilities_; }
};
