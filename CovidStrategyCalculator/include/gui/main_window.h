/* main_window.h
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
 * This file defines the MainWindow class which derives from QMainWindow.
 * MainWindow defines the layout of the application.
 */

#pragma once

#include "include/core/simulation.h"
#include "include/gui/efficacy_table.h"
#include "include/gui/result_log.h"
#include "include/gui/user_input/input_container.h"

#include <QMainWindow>
#include <QtCharts/QChartView>

class MainWindow : public QMainWindow {
    Q_OBJECT

    // The four components of COVIDStrategycalculator
    InputContainer *input_container;
    ResultLog *result_log;
    QtCharts::QChartView *chart_view; // chart for % relative risk profile and time-dependent diagnostic assay sens.
    EfficacyTable *efficacy_table;

  public:
    explicit MainWindow(QWidget *parent = 0); // constructor
    ~MainWindow() = default;                  // destructor

  private slots:
    void update_plot(Simulation *simulation);
    void update_result_log(Simulation *simulation);
    void update_efficacy_table(Simulation *simulation);
};
