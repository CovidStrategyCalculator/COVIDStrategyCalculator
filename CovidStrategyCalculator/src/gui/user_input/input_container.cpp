#include "include/gui/user_input/input_container.h"
#include "include/core/prevalence_estimator.h"
#include "include/gui/utils.h"

InputContainer::InputContainer(QWidget *parent) : QTabWidget(parent) {

    strategy_tab = new StrategyTab;
    parameters_tab = new ParametersTab;
    prevalence_tab = new PrevalenceTab;

    this->addTab(strategy_tab, tr("Strategy"));
    this->addTab(parameters_tab, tr("Parameters"));
    this->addTab(prevalence_tab, tr("Prevalence estimator"));
    this->setCurrentIndex(0);

    // mode incoming travelers is selected in the strategy tab
    connect(strategy_tab, &StrategyTab::jump_to_prevalence_tab, [=]() { this->setCurrentIndex(2); });
    // connect(strategy_tab, &StrategyTab::mode_is_2, [=]() { prevalence_tab->uncheck_all_use_case_boxes(); });
    connect(strategy_tab, &StrategyTab::mode_changed, [=]() { prevalence_tab->uncheck_all_use_case_boxes(); });

    // one of the use case checkboxes in the prevalence tab is checked
    connect(prevalence_tab, &PrevalenceTab::use_checkbox_checked, [=]() {
        strategy_tab->set_mode(2); // in case it wasn't yet
                                   //(in prevalence tab because clicked in tab menu; not by changing mode)
        strategy_tab->set_p_infectious_t0(prevalence_tab->p_infectious_t0());
        this->setCurrentIndex(0);
        strategy_tab->enable_run_button();
    });
    connect(prevalence_tab, &PrevalenceTab::all_boxes_are_unchecked, [=]() {
        if (strategy_tab->mode() == 2) {
            strategy_tab->disable_run_button();
        }
    });

    connect(prevalence_tab, &PrevalenceTab::disable_main_run_button, [=]() { strategy_tab->disable_run_button(); });
    connect(prevalence_tab, &PrevalenceTab::enable_main_run_button, [=]() { strategy_tab->enable_run_button(); });

    connect(prevalence_tab, &PrevalenceTab::run_prevalence_estimator, [=]() { run_prevalence_estimator(); });

    connect(strategy_tab, &StrategyTab::run_simulation, [=]() { run_simulation(); });
}

void InputContainer::run_prevalence_estimator() {
    PrevalenceEstimator *prevalence_simulation = new PrevalenceEstimator(parameters_tab, prevalence_tab);
    prevalence_tab->set_states(prevalence_simulation->compartment_states());
    prevalence_tab->set_probabilities(prevalence_simulation->phase_probabilities());
    prevalence_tab->update_layout();
}

void InputContainer::run_simulation() {
    Simulation *simulation = new Simulation(parameters_tab, strategy_tab, prevalence_tab);
    emit output_results(simulation);
}
