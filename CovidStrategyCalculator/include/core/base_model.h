#pragma once

#include <Eigen/Dense>
#include <vector>

class BaseModel {

  public:
    static const std::vector<int> sub_compartments; //{5, 1, 13, 1, 1} defined in cpp
    static const int n_compartments = 21;

    BaseModel(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
              Eigen::Vector<float, n_compartments> initial_states);

    Eigen::Vector<float, n_compartments> X0;
    Eigen::Vector<float, n_compartments> run_base(int time);
    Eigen::Vector<float, n_compartments> run_base(int time, Eigen::Vector<float, n_compartments> initial_states);

  private:
    float risk_posing_symptomatic_{};
    std::vector<float> tau_{};

    Eigen::Vector<float, n_compartments> rates_;
    Eigen::Matrix<float, n_compartments, n_compartments - 1> S_;
    Eigen::Matrix<float, n_compartments, n_compartments> A_;

    void set_rates();
    void set_S();
    void set_A();
};
