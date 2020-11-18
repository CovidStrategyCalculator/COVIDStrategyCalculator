#pragma once

#include "include/core/simulation.h"

#include <QTableWidget>

class ResultLog : public QTableWidget {
    Q_OBJECT

  public:
    explicit ResultLog(QWidget *parent = nullptr);

    void write_row_result_log(Simulation *simulation);
    bool event(QEvent *event);
};
