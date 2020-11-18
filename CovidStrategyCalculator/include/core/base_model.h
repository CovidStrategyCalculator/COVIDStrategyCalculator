#pragma once

#include <Eigen/Dense>
#include <vector>

class BaseModel {

  public:
    static const std::vector<int> sub_compartments; //{5, 1, 13, 1, 1} defined in cpp
    static const int n_compartments;

    BaseModel() = default;
    BaseModel(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
              Eigen::Vector<float, 21> initial_states);
    ~BaseModel() = default;

    Eigen::Vector<float, 21> X0;
    Eigen::Vector<float, 21> run_base(int time);
    Eigen::Vector<float, 21> run_base(int time, Eigen::Vector<float, 21> initial_states);

  private:
    float risk_posing_symptomatic_{};
    std::vector<float> tau_{};

    Eigen::Vector<float, 21> rates_;
    Eigen::Matrix<float, 21, 21 - 1> S_;
    Eigen::Matrix<float, 21, 21> A_;

    void set_rates();
    void set_S();
    void set_A();
};
