#pragma once

#include "include/core/simulation.h"

#include <QTableWidget>

class EfficacyTable : public QTableWidget {
    Q_OBJECT

  public:
    explicit EfficacyTable(QWidget *parent = nullptr);
    ~EfficacyTable() = default;

    void update(Simulation *simulation);
};
