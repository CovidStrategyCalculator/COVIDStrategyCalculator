#include "include/gui/main_window.h"
#include "include/gui/plot_area.h"

#include <Eigen/Dense>

#include <QDesktopWidget>
#include <QGridLayout>

#include <QScrollArea>
#include <vector>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    QSize size = QDesktopWidget().availableGeometry(this).size();

    int height = size.height();
    int width = size.width();

    QFont new_font = QDesktopWidget().font();
    new_font.setPointSize(8);
    new_font.setWeight(1);
    this->setFont(new_font);

    this->input_container = new InputContainer;
    int input_container_width =
        round(0.85 * input_container->minimumSizeHint().width()); // 85% to correct for exessive padding of QTabWidget
    int input_container_height =
        round(0.85 * input_container->minimumSizeHint().height()); // 85% to correct for exessive padding of QTabWidget

    input_container->setMinimumSize(input_container_width, input_container_height);
    input_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->result_log = new ResultLog;
    result_log->setMinimumSize(round(.98 * width) - input_container_width, input_container_height);
    result_log->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    this->chart_view = new QtCharts::QChartView;
    chart_view->setRenderHint(QPainter::Antialiasing);
    chart_view->setMinimumSize(round(.98 * width), round(1.15 * input_container_height));
    chart_view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->efficacy_table = new EfficacyTable;
    efficacy_table->setMinimumSize(round(.98 * width), round(0.25 * input_container_height));
    efficacy_table->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(input_container, &InputContainer::output_results, [=](Simulation *simulation) {
        update_plot(simulation);
        update_result_log(simulation);
        update_efficacy_table(simulation);
    });

    QHBoxLayout *upper_layout = new QHBoxLayout;
    upper_layout->addWidget(input_container);
    upper_layout->addWidget(result_log);
    upper_layout->addStretch();

    QVBoxLayout *lower_layout = new QVBoxLayout;
    lower_layout->setContentsMargins(0, 0, 0, 0);
    lower_layout->setSpacing(0);
    lower_layout->addWidget(chart_view);
    lower_layout->addWidget(efficacy_table);
    lower_layout->addStretch();

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addItem(upper_layout);
    main_layout->addItem(lower_layout);
    main_layout->addStretch();

    QWidget *central_widget = new QWidget;
    central_widget->setGeometry(QRect(0, 0, width, height));
    central_widget->setLayout(main_layout);

    QScrollArea *scroll_central = new QScrollArea(this);
    scroll_central->setWidget(central_widget);
    scroll_central->setWidgetResizable(true);
    scroll_central->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setCentralWidget(scroll_central);
}

void MainWindow::update_plot(Simulation *simulation) {
    PlotArea *plot_area = new PlotArea(simulation);
    chart_view->setChart(plot_area);
}

void MainWindow::update_result_log(Simulation *simulation) { result_log->write_row_result_log(simulation); }
void MainWindow::update_efficacy_table(Simulation *simulation) { efficacy_table->update(simulation); }
