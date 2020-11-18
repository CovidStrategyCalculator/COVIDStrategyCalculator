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
        detect_mean->append(time_sensitivity[j], assay_sensitivity(j, 0));
        detect_low->append(time_sensitivity[j], assay_sensitivity(j, 1));
        detect_high->append(time_sensitivity[j], assay_sensitivity(j, 2));
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

    // auto scaled = scale_risk_trajectories(mean, best_case, worst_case);

    for (int j = 0; j < risk.rows(); ++j) {
        risk_mean->append(time_risk[j], risk(j, 0));
        risk_low->append(time_risk[j], risk(j, 1));
        risk_high->append(time_risk[j], risk(j, 2));
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
    this->axes(Qt::Vertical).first()->setTitleText("Relative risk");

    QtCharts::QValueAxis *axisX = qobject_cast<QtCharts::QValueAxis *>(this->axes(Qt::Horizontal).first());
    axisX->setTickCount(time_sensitivity.size());

    QtCharts::QValueAxis *axisY = qobject_cast<QtCharts::QValueAxis *>(this->axes(Qt::Vertical).first());
    axisY->setRange(0, 1);

    QtCharts::QValueAxis *axisY2 = new QtCharts::QValueAxis;
    axisY2->setTitleText("Assay sensitivity");
    this->addAxis(axisY2, Qt::AlignRight);
    detect_area->attachAxis(axisY2);

    this->legend()->show();
    QFont f("Helvetica", 10);
    this->legend()->setFont(f);

    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->setBackgroundRoundness(0);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
