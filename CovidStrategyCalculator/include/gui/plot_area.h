/* plot_area.h
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
 * This file defines the PlotArea class which derives from QChart.
 * PlotArea handles the plotting of the % relative risk profile and time-dependent diagnostic assay sensitivity.
 */

#pragma once

#include "include/core/simulation.h"

#include <QtCharts/QChart>

class PlotArea : public QtCharts::QChart {
    Q_OBJECT

  public:
    PlotArea() = default;                      // constructor
    explicit PlotArea(Simulation *simulation); // constructor
    ~PlotArea() = default;                     // destructor
};
