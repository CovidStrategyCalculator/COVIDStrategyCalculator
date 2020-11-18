#pragma once

#include "include/core/simulation.h"
#include "include/gui/user_input/parameters_tab.h"
#include "include/gui/user_input/prevalence_tab.h"
#include "include/gui/user_input/strategy_tab.h"

#include <QTabWidget>

class InputContainer : public QTabWidget {
    Q_OBJECT

    StrategyTab *strategy_tab;
    ParametersTab *parameters_tab;
    PrevalenceTab *prevalence_tab;

    void run_prevalence_estimator();
    void run_simulation();

  public:
    explicit InputContainer(QWidget *parent = nullptr);

  signals:
    void output_results(Simulation *simulation);
};
