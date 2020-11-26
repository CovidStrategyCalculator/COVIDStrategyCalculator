/* plot_area.cpp
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
 * This file implements the PlotArea class which derives from QChart.
 * PlotArea handles the plotting of the % relative risk profile and time-dependent diagnostic assay sensitivity.
 */

#include "include/gui/plot_area.h"
#include "include/gui/utils.h"

#include <QGraphicsLayout>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChart>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

PlotArea::PlotArea(Simulation *simulation) : QtCharts::QChart(nullptr) {
    Eigen::MatrixXf risk = Utils::mid_min_max(simulation->relative_risk());
    Eigen::VectorXf time_risk = simulation->evaluation_points_with_tests();

    Eigen::MatrixXf assay_sensitivity = Utils::mid_min_max(simulation->temporal_assay_sensitivity());
    Eigen::VectorXf time_sensitivity = simulation->evaluation_points_without_tests();

    QtCharts::QLineSeries *detect_mean = new QtCharts::QLineSeries;
    QtCharts::QLineSeries *detect_low = new QtCharts::QLineSeries;
    QtCharts::QLineSeries *detect_high = new QtCharts::QLineSeries;

    for (int j = 0; j < assay_sensitivity.rows(); ++j) {
        detect_mean->append(time_sensitivity[j], assay_sensitivity(j, 0) * 100.);
        detect_low->append(time_sensitivity[j], assay_sensitivity(j, 1) * 100.);
        detect_high->append(time_sensitivity[j], assay_sensitivity(j, 2) * 100.);
    }
    QtCharts::QAreaSeries *detect_area = new QtCharts::QAreaSeries(detect_low, detect_high);
    detect_area->setName("Assay sensitivity");

    QColor detect_color(140, 129, 152, 128);
    QPen detect_pen(detect_color);
    detect_area->setPen(detect_pen);
    QBrush detect_brush(detect_color);
    detect_area->setBrush(detect_brush);

    this->addSeries(detect_mean);
    this->addSeries(detect_area);
    this->legend()->markers(detect_mean)[0]->setVisible(false);

    QtCharts::QLineSeries *risk_low = new QtCharts::QLineSeries;
    QtCharts::QLineSeries *risk_mean = new QtCharts::QLineSeries;
    QtCharts::QLineSeries *risk_high = new QtCharts::QLineSeries;
    risk_mean->setName("Relative risk");

    for (int j = 0; j < risk.rows(); ++j) {
        risk_mean->append(time_risk[j], risk(j, 0) * 100.);
        risk_low->append(time_risk[j], risk(j, 1) * 100.);
        risk_high->append(time_risk[j], risk(j, 2) * 100.);
    }
    QtCharts::QAreaSeries *risk_area = new QtCharts::QAreaSeries(risk_low, risk_high);

    QColor risk_color(255, 128, 128, 128);
    QPen risk_pen(risk_color);
    risk_area->setPen(risk_pen);
    QBrush risk_brush(risk_color);
    risk_area->setBrush(risk_brush);
    QPen red_pen(Qt::red);
    risk_mean->setPen(red_pen);

    this->addSeries(risk_area);
    this->addSeries(risk_mean);
    this->legend()->markers(risk_area)[0]->setVisible(false);

    this->createDefaultAxes();
    switch (simulation->get_mode()) {
    case 0:
        this->axes(Qt::Horizontal).first()->setTitleText("Days since infection");
        break;
    case 1:
        this->axes(Qt::Horizontal).first()->setTitleText("Days since symptom onset");
        break;
    case 2:
        this->axes(Qt::Horizontal).first()->setTitleText("Days since entry");
        break;
    }
    this->axes(Qt::Vertical).first()->setTitleText("Relative risk [%]");

    QtCharts::QValueAxis *axisX = qobject_cast<QtCharts::QValueAxis *>(this->axes(Qt::Horizontal).first());
    axisX->setTickCount(time_sensitivity.size());

    QtCharts::QValueAxis *axisY = qobject_cast<QtCharts::QValueAxis *>(this->axes(Qt::Vertical).first());
    axisY->setRange(0, 100);

    QtCharts::QValueAxis *axisY2 = new QtCharts::QValueAxis;
    axisY2->setTitleText("Assay sensitivity [%]");
    this->addAxis(axisY2, Qt::AlignRight);
    detect_area->attachAxis(axisY2);

    this->legend()->show();
    QFont f("Helvetica", 10);
    this->legend()->setFont(f);

    QFont font;
    font.setPixelSize(12);
    axisX->setLabelsFont(font);
    axisY->setLabelsFont(font);
    axisY2->setLabelsFont(font);

    axisX->setTitleFont(font);
    axisY->setTitleFont(font);
    axisY2->setTitleFont(font);
}
