#pragma once

#include "include/core/simulation.h"

#include <QtCharts/QChart>

class PlotArea : public QtCharts::QChart {
    Q_OBJECT

  public:
    explicit PlotArea(Simulation *simulation);
};
