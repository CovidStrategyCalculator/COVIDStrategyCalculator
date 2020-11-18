#include "include/gui/main_window.h"
#include "include/gui/plot_area.h"

#include <Eigen/Dense>

#include <QDesktopWidget>
#include <QGridLayout>

#include <QScrollArea>
#include <vector>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    QSize size = QDesktopWidget().availableGeometry(this).size();
    resize(size);

    this->input_container = new InputContainer;
    QScrollArea *scroll_tab = new QScrollArea(this);
    scroll_tab->setWidget(input_container);
    scroll_tab->setWidgetResizable(true);
    scroll_tab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->result_log = new ResultLog;
    QScrollArea *scroll_log = new QScrollArea(this);
    scroll_log->setWidget(result_log);
    scroll_log->setWidgetResizable(true);
    scroll_log->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->chart_view = new QtCharts::QChartView;
    chart_view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    chart_view->setRenderHint(QPainter::Antialiasing);

    this->efficacy_table = new EfficacyTable;
    efficacy_table->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    connect(input_container, &InputContainer::output_results, [=](Simulation *simulation) {
        update_plot(simulation);
        update_result_log(simulation);
        update_efficacy_table(simulation);
    });

    QHBoxLayout *upper_layout = new QHBoxLayout;
    upper_layout->addWidget(scroll_tab);
    upper_layout->addWidget(scroll_log);

    QVBoxLayout *lower_layout = new QVBoxLayout;
    lower_layout->setContentsMargins(0, 0, 0, 0);
    lower_layout->setSpacing(0);
    lower_layout->addWidget(chart_view, 90);
    lower_layout->addWidget(efficacy_table, 5);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addItem(upper_layout);
    main_layout->addItem(lower_layout);

    QWidget *central_widget = new QWidget;
    central_widget->setLayout(main_layout);
    this->setCentralWidget(central_widget);
}

void MainWindow::update_plot(Simulation *simulation) {
    PlotArea *plot_area = new PlotArea(simulation);
    chart_view->setChart(plot_area);
}

void MainWindow::update_result_log(Simulation *simulation) { result_log->write_row_result_log(simulation); }
void MainWindow::update_efficacy_table(Simulation *simulation) { efficacy_table->update(simulation); }
