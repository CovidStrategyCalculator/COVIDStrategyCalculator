/* result_log.cpp
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
 * This file implements the ResultLog class which derives from QTableWidget.
 * ResultLog handles the layout and updates to the results table.
 */

#include "include/gui/result_log.h"
#include "include/gui/utils.h"

#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>

ResultLog::ResultLog(QWidget *parent) : QTableWidget(0, 11, parent) {
    this->setHorizontalHeaderLabels((QStringList() << "mode"
                                                   << "sympt screening\n(perc asympt)"
                                                   << "adherence\n[%]"
                                                   << "time passed\n[days]"
                                                   << "quarantine/\nisolation\n[days]"
                                                   << "test\n[days]"
                                                   << "test type"
                                                   << "P(infectious)\nstart"
                                                   << "P(infectious)\nend"
                                                   << "relative risk\n[%]"
                                                   << "risk reduction\n[%]"));

    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 8pt; }");
    connect(this->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(resizeRowsToContents()));
}

void ResultLog::write_row_result_log(Simulation *simulation) {

    this->insertRow(0);

    std::map<int, std::string> mode_map_int{{0, "contact management"}, {1, "isolation"}, {2, "incoming travelers"}};
    this->setItem(0, 0, new QTableWidgetItem(QString(mode_map_int[simulation->get_mode()].c_str())));

    QString boolText = simulation->get_symptomatic_screening() ? "yes" : "no";
    if (simulation->get_symptomatic_screening()) {
        this->setItem(0, 1,
                      new QTableWidgetItem(boolText + " (" +
                                           QString::number(simulation->get_fraction_asymptomatic() * 100) + "%)"));
    } else {
        this->setItem(0, 1, new QTableWidgetItem(boolText));
    }

    this->setItem(0, 2, new QTableWidgetItem(QString::number(simulation->get_adherence() * 100.)));
    this->setItem(0, 3, new QTableWidgetItem(QString::number(simulation->get_t_offset())));
    this->setItem(0, 4, new QTableWidgetItem(QString::number(simulation->get_last_t())));

    if (simulation->get_t_test().size()) {
        QString days{};
        for (int day : simulation->get_t_test()) {
            days += (QString::number(day - simulation->get_t_offset()) + ", ");
        }
        days.chop(2);
        this->setItem(0, 5, new QTableWidgetItem(days));

        std::map<int, std::string> test_type_map{{0, "PCR"}, {1, "Antigen"}};
        this->setItem(0, 6, new QTableWidgetItem(test_type_map[simulation->get_test_type()].c_str()));
    } else {
        this->setItem(0, 5, new QTableWidgetItem());
        this->setItem(0, 6, new QTableWidgetItem());
    }

    this->setItem(0, 7, new QTableWidgetItem(QString::number(simulation->get_p_infectious_t0(), 'f', 2)));

    Eigen::MatrixXf risk_reduction = Utils::mid_min_max(simulation->risk_reduction())(Eigen::last, Eigen::all);
    Eigen::MatrixXf fold_risk_reduction =
        Utils::mid_min_max(simulation->fold_risk_reduction())(Eigen::last, Eigen ::all);

    Eigen::VectorXf relative_risk(3);
    relative_risk = Utils::mid_min_max((100. / fold_risk_reduction(0, 0)), (100. / fold_risk_reduction(0, 1)),
                                       (100. / fold_risk_reduction(0, 2)));

    Eigen::VectorXf p_infectious_tend = simulation->get_p_infectious_tend();
    p_infectious_tend = Utils::mid_min_max(p_infectious_tend(0), p_infectious_tend(1), p_infectious_tend(2));

    this->setItem(0, 8,
                  new QTableWidgetItem(Utils::safeguard_probability(p_infectious_tend(0), 2) + " (" +
                                       Utils::safeguard_probability(p_infectious_tend(1), 2) + ", " +
                                       Utils::safeguard_probability(p_infectious_tend(2), 2) + ")"));

    this->setItem(0, 9,
                  new QTableWidgetItem(Utils::safeguard_inf(relative_risk(0), 2) + " (" +
                                       Utils::safeguard_inf(relative_risk(1), 2) + ", " +
                                       Utils::safeguard_inf(relative_risk(2), 2) + ")"));

    this->setItem(0, 10,
                  new QTableWidgetItem(Utils::safeguard_probability(risk_reduction(0, 0) * 100., 2) + " (" +
                                       Utils::safeguard_probability(risk_reduction(0, 1) * 100., 2) + ", " +
                                       Utils::safeguard_probability(risk_reduction(0, 2) * 100., 2) + ")"));

    for (int i = 0; i < 11; ++i) {
        this->item(0, i)->setFlags(this->item(0, i)->flags() & ~Qt::ItemIsEditable);
    }
}

bool ResultLog::event(QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->matches(QKeySequence::Copy)) {
            QString copied_text;
            QModelIndexList current_selected_indexs = this->selectedIndexes();
            int current_row = current_selected_indexs.at(0).row();
            for (int i = 0; i < current_selected_indexs.count(); i++) {
                if (current_row != current_selected_indexs.at(i).row()) {
                    current_row = current_selected_indexs.at(i).row();
                    copied_text.append("\n");
                    copied_text.append(current_selected_indexs.at(i).data().toString());
                    continue;
                }
                if (0 != i) {
                    copied_text.append("\t");
                }
                copied_text.append(current_selected_indexs.at(i).data().toString());
            }
            copied_text.append("\n");
            QApplication::clipboard()->setText(copied_text);
            event->accept();
            return true;
        }
    }
    return QTableView::event(event);
}
