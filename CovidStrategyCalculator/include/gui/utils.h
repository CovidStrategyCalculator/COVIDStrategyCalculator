#pragma once

#include <Eigen/Dense>

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QString>

namespace Utils {
QDoubleSpinBox *create_DoubleSpinBox(double value, double minimum, double maximum, int presicion);
QSpinBox *create_SpinBox(int value, int minimum, int maximum);

Eigen::VectorXf mid_min_max(float value1, float value2, float value3);
Eigen::MatrixXf mid_min_max(Eigen::MatrixXf matrix);
QString safeguard_inf(float value, int precision);
QString safeguard_probability(float value, int precision);
std::vector<QString> safeguard_inf(Eigen::VectorXf values, int precision);
std::vector<QString> safeguard_probability(Eigen::VectorXf values, int precision);
} // namespace Utils
