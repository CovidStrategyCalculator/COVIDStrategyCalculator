#pragma once

#include "include/gui/user_input/parameters_tab.h"

#include <Eigen/Dense>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class PrevalenceTab : public QWidget {
    Q_OBJECT

    QSpinBox *reports4_;
    QSpinBox *reports3_;
    QSpinBox *reports2_;
    QSpinBox *reports1_;
    QSpinBox *reports0_;

    QDoubleSpinBox *percent_of_cases_detected_;

    QLabel *percent_of_population4_;
    QLabel *percent_of_population3_;
    QLabel *percent_of_population2_;
    QLabel *percent_of_population1_;
    QLabel *percent_of_population0_;

    QGroupBox *result_box_;

    QLabel *result_predetect_;
    QLabel *result_presympt_;
    QLabel *result_sympt_;
    QLabel *result_postinf_;
    QLabel *result_total_prevalence_;
    QLabel *result_inf_prevalence_;

    QLabel *result_predetect_range_;
    QLabel *result_presympt_range_;
    QLabel *result_sympt_range_;
    QLabel *result_postinf_range_;
    QLabel *result_total_prevalence_range_;
    QLabel *result_inf_prevalence_range_;

    Eigen::MatrixXf compartment_states_;  // n_compartment x 3: {typical, lower , upper}
    Eigen::MatrixXf phase_probabilities_; // 5x3 (predetect, presympt, sympt, postinf, risknode),(typical, lower, upper)

    std::vector<QCheckBox *> use_for_main_simulation_;
    int use_case; // column index for states_ and probabilities_ matrices based on the state of the checkboxes
                  // in use_for_main_simulation_
    bool use_prevalence_estimation_{false}; // indicator variable for whether one of the use_case boxes is checked
    QPushButton *run_button_;

    void initialize_member_variables();
    void set_layout();
    void estimate_prevalence();
    void state_of_use_checkbox_changed(); // check state of one of the use checkboxes changed

  public:
    explicit PrevalenceTab(QWidget *parent = nullptr);

    // read-only getter functions
    float p_infectious_t0() const { return phase_probabilities_.col(use_case)(Eigen::seq(0, 2)).sum(); }
    Eigen::VectorXf initial_states() const { return compartment_states_.col(use_case); }
    std::vector<float> incidence();
    bool use_prevalence_estimation() {
        return (use_for_main_simulation_[0]->isChecked() || use_for_main_simulation_[1]->isChecked()) ||
               use_for_main_simulation_[2]->isChecked();
    }

    // setters
    void set_states(Eigen::MatrixXf matrix) { compartment_states_ = matrix; }
    void set_probabilities(Eigen::MatrixXf matrix) { phase_probabilities_ = matrix; }

    // functions to communicate with the InputContainer
    void uncheck_all_use_case_boxes();
    void update_layout();

  signals:
    void use_checkbox_checked();
    void all_boxes_are_unchecked();
    void disable_main_run_button();
    void enable_main_run_button();
    void jump_to_strategy_tab();
    void run_prevalence_estimator();
};
