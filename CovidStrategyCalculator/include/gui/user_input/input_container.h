/* input_container.h
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
 * This file defines the InputContainer class for the user input which derives from QTabWidget.
 * The objective of the InputContainer is to handle the interaction between the different user input tabs
 * and to initialize the simulation and/or the prevalence estimation with the user input.
 */

#pragma once

#include "include/core/simulation.h"
#include "include/gui/user_input/parameters_tab.h"
#include "include/gui/user_input/prevalence_tab.h"
#include "include/gui/user_input/strategy_tab.h"

#include <QTabWidget>

class InputContainer : public QTabWidget {
    Q_OBJECT

    // the three user input tabs
    StrategyTab *strategy_tab;
    ParametersTab *parameters_tab;
    PrevalenceTab *prevalence_tab;

    // functions to initialize and execute the simulation and prevalence estimator based on the user input
    void run_prevalence_estimator();
    void run_simulation();

  public:
    explicit InputContainer(QWidget *parent = nullptr); // constructor

  signals:
    /* signal to emit after running the simulation, used to pass the simulation object to
     *  the plotting and reporting functions
     */
    void output_results(Simulation *simulation);
};
