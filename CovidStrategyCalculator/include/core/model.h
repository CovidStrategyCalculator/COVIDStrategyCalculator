#pragma once

#include "include/core/base_model.h"

#include <Eigen/Dense>
#include <vector>

class Model : public BaseModel {

    int t_end{};
    std::vector<int> t_test{};

    float specificity{};
    float sensitivity{};
    Eigen::VectorXf false_ommision_rate;
    void set_false_ommision_rate();

    Eigen::MatrixXf run_no_test(int time);

  public:
    Model() = default;
    Model(std::vector<float> residence_times, float risk_posing_fraction_symptomatic_phase,
          Eigen::VectorXf initial_states, int time, std::vector<int> test_indices, float test_sensitivity,
          float test_specificity);
    ~Model() = default;

    // no test or symptomatic screening
    Model(std::vector<float> residence_times, Eigen::VectorXf initial_states, int time);

    Eigen::MatrixXf run();
    Eigen::MatrixXf run_no_test();
    Eigen::VectorXf integrate(Eigen::MatrixXf X);
    void set_t_end(int new_t_end) { t_end = new_t_end; }
    // Eigen::MatrixXf group_by_phase(Eigen::MatrixXf X);
};
