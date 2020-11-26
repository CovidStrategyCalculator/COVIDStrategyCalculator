/* efficacy_table.cpp
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
 * This file implements the EfficacyTable class which derives from QTableWidget.
 * The EfficacyTable class displays the diagnostic test efficacy P(infectious|positive test).
 */

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

    QStringList header_labels;
    for (int i = 0; i < time.size(); ++i) {
        header_labels << QString::number(time(i));
    }

    this->setHorizontalHeaderLabels(header_labels);
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
