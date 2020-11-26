/* utils.h
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
 * This file defines the namespace Utils and several helper utilities therein.
 */

#pragma once

#include <Eigen/Dense>

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QString>

namespace Utils {
QDoubleSpinBox *create_DoubleSpinBox(double value, double minimum, double maximum, int presicion);
QSpinBox *create_SpinBox(int value, int minimum, int maximum);

Eigen::VectorXf mid_min_max(float value1, float value2, float value3); // sort three float in ascending order
Eigen::MatrixXf mid_min_max(Eigen::MatrixXf matrix);                   // sort each row of a matrix in ascending order
QString safeguard_inf(float value, int precision);         // display > 1e12 when approaching inf probabilities
QString safeguard_probability(float value, int precision); // cap
std::vector<QString> safeguard_inf(Eigen::VectorXf values, int precision);
std::vector<QString> safeguard_probability(Eigen::VectorXf values, int precision);
} // namespace Utils
