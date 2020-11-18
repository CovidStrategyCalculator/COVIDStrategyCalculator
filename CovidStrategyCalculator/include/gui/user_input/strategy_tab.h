#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class StrategyTab : public QWidget {
    Q_OBJECT

    QComboBox *mode_;
    QSpinBox *time_delay_;
    QSpinBox *end_of_strategy_;
    QDoubleSpinBox *p_infectious_t0_;
    QCheckBox *use_symptomatic_screening_;
    QSpinBox *expected_adherence_;
    QGroupBox *test_days_box_;
    QComboBox *test_type_;
    QPushButton *run_button_;

    std::vector<QCheckBox *> test_days_checkable_boxes{};
    std::vector<bool> test_days_checkable_boxes_states{};
    void set_layout_test_days_box();
    void update_test_days_box();

    void initialize_member_variables();
    void set_layout();

    void configure_options_based_on_mode(int current_mode);

  public:
    // TODO: is explicit needed here? use =default?
    explicit StrategyTab(QWidget *parent = nullptr);

    // read-only getter functions
    int mode() const { return mode_->currentIndex(); }
    QString mode_string() const { return mode_->currentText(); }
    int time_delay() const { return time_delay_->value(); }
    int end_of_strategy() const { return end_of_strategy_->value(); }
    float p_infectious_t0() const { return p_infectious_t0_->value(); }
    bool use_symptomatic_screening() const { return use_symptomatic_screening_->isChecked(); }
    float expected_adherence() const { return expected_adherence_->value() / 100.; } // scales percent to fraction
    int test_type() const { return test_type_->currentIndex(); }
    QString test_type_string() const { return test_type_->currentText(); }
    std::vector<int> test_moments() const; // indices of placed tests; 0-indexed, also in case of non-zero time delay

    // setter function
    void set_p_infectious_t0(float risk) { p_infectious_t0_->setValue(risk); }
    void set_mode(int index) { mode_->setCurrentIndex(index); }
    void enable_run_button() { run_button_->setEnabled(true); };
    void disable_run_button() { run_button_->setEnabled(false); };

  signals:
    void jump_to_prevalence_tab();
    void mode_is_2();
    void mode_changed();
    void run_simulation();
};
