#ifndef SIMULATION_H
#define SIMULATION_H

#include "mainwindow.h"

#include <Eigen/Dense>

#include <QCheckBox>
#include <QMainWindow>
#include <QTableWidget>

#include <QtCharts/QChartView>

class MainWindow;

class Simulation : public QObject {
  Q_OBJECT
public:
  explicit Simulation(MainWindow *parent);
  explicit Simulation(MainWindow *parent, Eigen::MatrixXf init_states,
                      float prerisk);
  void run();
  std::tuple<std::vector<Eigen::MatrixXf>, std::vector<Eigen::MatrixXf>>
  run_prevalence(std::vector<float> week_incidences);
  void output_results();

protected:
  // model structure
  MainWindow *m_parent = nullptr;
  static std::vector<int> sub_compartments;
  static int nr_compartments;
  static float t_inf;

  // strategy tab
  float pre_test_infect_prob{1.};
  int mode;
  std::string mode_str;
  int time_passed;
  int quarantine;
  bool use_symptomatic_screening;
  float adherence;
  std::vector<int> t_test{};
  QString test_type;

  // parameters tab
  std::vector<float> residence_times_mean;
  std::vector<float> residence_times_lev;
  std::vector<float> residence_times_uev;

  float sensitivity;
  float specificity;

  float fraction_asymtomatic;

  // simulation
  Eigen::VectorXf initial_states;
  Eigen::MatrixXf X_mean;
  Eigen::MatrixXf X_lev;
  Eigen::MatrixXf X_uev;
  Eigen::MatrixXf result_matrix_mean;
  Eigen::MatrixXf result_matrix_lev;
  Eigen::MatrixXf result_matrix_uev;
  Eigen::MatrixXf assay_detectibility_worst_case;
  Eigen::MatrixXf assay_detectibility_best_case;
  Eigen::MatrixXf assay_detectibility_mean_case;

  Eigen::MatrixXf risk_reduction_trajectory_worst_case;
  Eigen::MatrixXf risk_reduction_trajectory_best_case;
  Eigen::MatrixXf risk_reduction_trajectory_mean_case;

  float fold_RR_mean, fold_RR_lev, fold_RR_uev;
  std::vector<int> time_for_plot;

  void collect_data(MainWindow *parent);
  std::vector<int> collect_t_test(std::vector<QCheckBox *> boxes);

  Eigen::VectorXf calc_rates(std::vector<float> times, std::vector<int> comp);
  Eigen::VectorXf FOR_vector(std::vector<int> comp);
  Eigen::MatrixXf calc_S(int n);
  Eigen::MatrixXf calc_A(Eigen::MatrixXf S_, Eigen::VectorXf r,
                         std::vector<int> comp);
  Eigen::MatrixXf calc_X(int delay, int qrntn, Eigen::MatrixXf A_,
                         Eigen::VectorXf states);
  float calc_risk_at_T(float time_T, Eigen::VectorXf X0, Eigen::MatrixXf A);
  Eigen::MatrixXf assemble_phases(Eigen::MatrixXf X_, std::vector<int> comp);

  Eigen::MatrixXf states_matrix_to_risk_trajectory(Eigen::MatrixXf X,
                                                   Eigen::MatrixXf A,
                                                   std::vector<int> days);
  std::tuple<Eigen::MatrixXf, Eigen::MatrixXf, Eigen::MatrixXf>
  scale_risk_trajectories(Eigen::MatrixXf risk_node_mean_case,
                          Eigen::MatrixXf risk_node_best_case,
                          Eigen::MatrixXf risk_node_worst_case);
  void reorder_risk_trajectories();

  QString safeguard_inf(float value, int precision);
  QString safeguard_probability(float probability, int precision, int max);
  QtCharts::QChartView *create_plot(Eigen::MatrixXf detectable,
                                    Eigen::MatrixXf mean,
                                    Eigen::MatrixXf best_case,
                                    Eigen::MatrixXf worst_case,
                                    std::vector<int> time_range_for_plot);
  QTableWidget *create_efficacy_table();

  void create_result_log();
  void write_row_result_log(QTableWidget *);
  void update_result_log();
  std::tuple<float, float, float> mid_min_max(float value1, float value2,
                                              float value3);

  float calculate_strategy_result(Eigen::MatrixXf matrix);
  Eigen::MatrixXf calculate_assay_sensitivity();
  Eigen::MatrixXf calculate_test_efficay();

  std::tuple<Eigen::MatrixXf, std::vector<int>, float>
  calculate_strategy_with_test(Eigen::MatrixXf A);
signals:

public slots:
};

#endif // SIMULATION_H
