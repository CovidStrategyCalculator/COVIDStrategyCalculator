/* strategy_tab.h
 * Written by Wiep van der Toorn.
 *
 * This file is part of COVIDStrategycalculator.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 *
 * This file defines the StrategyTab class for the strategy-related user input and derives from QWidget.
 * The StrategyTab provides the first tab of the user input field and allows the user to design and evaluate a custom
 * NPI strategy.
 */

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

    // private member variables corresponding to the input fields / buttons in the widget
    QComboBox *mode_;
    QSpinBox *time_delay_;
    QSpinBox *end_of_strategy_;
    QDoubleSpinBox *p_infectious_t0_;
    QCheckBox *use_symptomatic_screening_;
    QSpinBox *expected_adherence_;
    QGroupBox *test_days_box_;
    QComboBox *test_type_;
    QPushButton *run_button_;

    // variables and functions for the placements of diagnostic test
    std::vector<QCheckBox *> test_days_boxes{};
    std::vector<bool> test_days_boxes_states{};
    void set_layout_test_days_box();
    void update_test_days_box();

    // functions to initialize the widget
    void initialize_member_variables();
    void set_layout();

    void configure_options_based_on_mode(int current_mode);

  public:
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
    void mode_is_2(); // signals that mode 'prevalence estimation' was selected
    void mode_changed();
    void run_simulation();
};
