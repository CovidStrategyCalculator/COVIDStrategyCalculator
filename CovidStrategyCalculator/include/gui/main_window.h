#pragma once

#include "include/core/simulation.h"
#include "include/gui/efficacy_table.h"
#include "include/gui/result_log.h"
#include "include/gui/user_input/input_container.h"

#include <QMainWindow>
#include <QtCharts/QChartView>

class MainWindow : public QMainWindow {
    Q_OBJECT

    InputContainer *input_container;
    ResultLog *result_log;
    QtCharts::QChartView *chart_view;
    EfficacyTable *efficacy_table;

  public:
    explicit MainWindow(QWidget *parent = 0);

  private slots:
    void update_plot(Simulation *simulation);
    void update_result_log(Simulation *simulation);
    void update_efficacy_table(Simulation *simulation);
};
