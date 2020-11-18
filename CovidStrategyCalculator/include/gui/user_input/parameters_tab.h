#pragma once

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QWidget>

class ParametersTab : public QWidget {
    Q_OBJECT

    QDoubleSpinBox *incubation_mean_;
    QDoubleSpinBox *incubation_upper_;
    QDoubleSpinBox *incubation_lower_;
    QDoubleSpinBox *symptomatic_mean_;
    QDoubleSpinBox *symptomatic_upper_;
    QDoubleSpinBox *symptomatic_lower_;
    QDoubleSpinBox *postinfectious_;
    QDoubleSpinBox *percentage_predetection_;
    QDoubleSpinBox *percentage_asymptomatic_;
    QDoubleSpinBox *pcr_sens_;
    QDoubleSpinBox *pcr_spec_;
    QDoubleSpinBox *relative_rdt_sens_;
    QPushButton *reset_button_;

    std::map<std::string, float> default_values{{"duration_incubation_lower", 5.60},
                                                {"duration_incubation_mean", 6.77},
                                                {"duration_incubation_upper", 7.99},
                                                {"percentage_of_incubation_predetection", 42.2},
                                                {"duration_symptomatic_lower", 2.79},
                                                {"duration_symptomatic_mean", 7.50},
                                                {"duration_symptomatic_upper", 11.47},
                                                {"percentage_asymptomatic", 20},
                                                {"duration_postinfectious_mean", 8},
                                                {"PCR_sensitivity", 80},
                                                {"PCR_specificity", 99.99},
                                                {"relative_RDT_sensitivity", 85}};

    void initialize_member_variables();
    void set_layout();
    void reset_defaults();

  public:
    explicit ParametersTab(QWidget *parent = nullptr);

    // read-only getter functions
    float predetection_mean() const { return percentage_predetection_->value() / 100 * incubation_mean_->value(); }
    float predetection_upper() const { return percentage_predetection_->value() / 100 * incubation_upper_->value(); }
    float predetection_lower() const { return percentage_predetection_->value() / 100 * incubation_lower_->value(); }

    float presymptomatic_mean() const {
        return (1 - percentage_predetection_->value() / 100) * incubation_mean_->value();
    }
    float presymptomatic_upper() const {
        return (1 - percentage_predetection_->value() / 100) * incubation_upper_->value();
    }
    float presymptomatic_lower() const {
        return (1 - percentage_predetection_->value() / 100) * incubation_lower_->value();
    }

    float symptomatic_mean() const { return symptomatic_mean_->value(); }
    float symptomatic_upper() const { return symptomatic_upper_->value(); }
    float symptomatic_lower() const { return symptomatic_lower_->value(); }

    float postinfectious() const { return postinfectious_->value(); }

    float fraction_asymptomatic() const {
        return percentage_asymptomatic_->value() / 100.;
    } // scales percent to probability

    float pcr_sensitivity() const { return pcr_sens_->value() / 100.; }                   // percent to probability
    float pcr_specificity() const { return pcr_spec_->value() / 100.; }                   // percent to probability
    float relative_rdt_sensitivity() const { return relative_rdt_sens_->value() / 100.; } // percent to probability
};
