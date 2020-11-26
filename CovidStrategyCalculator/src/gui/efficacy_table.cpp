#include "include/gui/efficacy_table.h"
#include "include/gui/utils.h"

#include <QHeaderView>

EfficacyTable::EfficacyTable(QWidget *parent) : QTableWidget(parent) {
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(this->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(resizeRowsToContents()));
}

void EfficacyTable::update(Simulation *simulation) {

    this->clear();

    Eigen::MatrixXf efficacy = Utils::mid_min_max(simulation->test_efficacy());
    Eigen::VectorXf time = simulation->evaluation_points_without_tests();

    this->setColumnCount(time.size());
    this->setRowCount(1);

    QStringList h_labels;
    for (int i = 0; i < time.size(); ++i) {
        h_labels << QString::number(time(i));
    }

    this->setHorizontalHeaderLabels(h_labels);
    this->setVerticalHeaderLabels((QStringList() << "P(infectious | positive test)"));

    for (int i = 0; i < time.size(); ++i) {
        this->setItem(0, i,
                      new QTableWidgetItem(Utils::safeguard_probability(efficacy(i, 0), 3) + +"  (" +
                                           Utils::safeguard_probability(efficacy(i, 1), 3) + ", " +
                                           Utils::safeguard_probability(efficacy(i, 2), 3) + ")"));

        this->item(0, i)->setFlags(this->item(0, i)->flags() & ~Qt::ItemIsEditable);
    }
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
