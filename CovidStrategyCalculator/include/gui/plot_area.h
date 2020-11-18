#pragma once

#include "include/core/simulation.h"

#include <QtCharts/QChart>

class PlotArea : public QtCharts::QChart {
    Q_OBJECT

  public:
    PlotArea() = default;
    explicit PlotArea(Simulation *simulation);
    ~PlotArea() = default;
};
