#include "include/gui/utils.h"

/**
 * Create a QDoubleSpinBox instance with a given default value, min, max and precision.
 *
 * @param value: default value of spinbox.
 * @param minimum: the minimum value of the spinbox.
 * @param minimum: the minimum value of the spinbox.
 * @param precision: the number of decimals of the spinbox.
 * @return QDoubleSpinBox with the desired properties.
 */
QDoubleSpinBox *Utils::create_DoubleSpinBox(double value, double minimum, double maximum, int presicion) {
    QDoubleSpinBox *box = new QDoubleSpinBox;
    box->setMinimum(minimum);
    box->setMaximum(maximum);
    box->setDecimals(presicion);
    box->setValue(value);
    box->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    return box;
};

/**
 * Create a QSpinBox instance with a given default value, min and max.
 *
 * @param value: default value of spinbox.
 * @param minimum: the minimum value of the spinbox.
 * @param minimum: the minimum value of the spinbox.
 * @return QSpinBox with the desired properties.
 */
QSpinBox *Utils::create_SpinBox(int value, int minimum, int maximum) {
    QSpinBox *box = new QSpinBox;
    box->setMinimum(minimum);
    box->setMaximum(maximum);
    box->setValue(value);
    box->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    return box;
};

/**
 * Sort three floats in ascending order.
 *
 * @param value1, value2, value3: the values to be sorted.
 * @return Eigen::VectorXf of sorted values.
 */
Eigen::VectorXf Utils::mid_min_max(float value1, float value2, float value3) {
    std::vector<float> v{value1, value2, value3};

    int maxElementIndex = std::max_element(v.begin(), v.end()) - v.begin();
    int minElementIndex = std::min_element(v.begin(), v.end()) - v.begin();
    int midElementIndex = 3 - maxElementIndex - minElementIndex;
    if (maxElementIndex == minElementIndex) {
        midElementIndex = 0;
    }

    Eigen::VectorXf result(3);
    result[0] = v[midElementIndex];
    result[1] = v[minElementIndex];
    result[2] = v[maxElementIndex];

    return result;
}

/**
 * Sort rows of a matrix in ascending order.
 *
 * @param Eigen::MatrixXf matrix of shape (n,3) containing values to be sorted.
 * @return Eigen::MatrixXf matrix with sorted rows.
 */
Eigen::MatrixXf Utils::mid_min_max(Eigen::MatrixXf matrix) {

    Eigen::MatrixXf result(matrix.rows(), 3);

    for (int i = 0; i < matrix.rows(); ++i) {
        Eigen::VectorXf ordered = Utils::mid_min_max(matrix(i, 0), matrix(i, 1), matrix(i, 2));
        ordered.resize(3, 1);
        result.row(i) = ordered;
    }
    return result;
}

/**
 * Safeguard probability conversion from float to string to never display 'inf'. Instead displays >1e12.
 *
 * @param value: the probability to be converted to a string.
 * @param int: the number of decimals to display in the conversion string.
 * @return QString of the guarded probability .
 */
QString Utils::safeguard_inf(float value, int precision) {
    QString rounded = QString::number(value, 'f', precision);

    if (rounded == QString("inf")) {
        return QString(">1e12");
    }
    return rounded;
}

/**
 * Safeguard probability conversion from float to string to never display 1 or 0. .
 *
 * @param value: the probability to be converted to a string.
 * @param int: the number of decimals to display in the conversion string.
 * @return QString of the guarded probability .
 */
QString Utils::safeguard_probability(float value, int precision) {
    QString rounded = QString::number(value, 'f', precision);

    switch (precision) {
    case 1:
        if (rounded == QString("0.0")) {
            return QString("<0.1");
        } else if (rounded == QString("1.0")) {
            return QString(">0.9");
        } else if (rounded == QString("100.0")) {
            return QString(">99.9");
        } else
            return rounded;

    case 2:
        if (rounded == QString("0.00")) {
            return QString("<0.01");
        } else if (rounded == QString("1.00")) {
            return QString(">0.99");
        } else if (rounded == QString("100.00")) {
            return QString(">99.99");
        } else
            return rounded;

    case 3:
        if (rounded == QString("0.000")) {
            return QString("<0.001");
        } else if (rounded == QString("1.000")) {
            return QString(">0.999");
        } else if (rounded == QString("100.000")) {
            return QString(">99.999");
        } else
            return rounded;
    }
    return rounded;
}

/**
 * Safeguard probability conversion from float to string to never display 1 or 0. .
 *
 * @param values: vector of probability to be converted to a string.
 * @param int: the number of decimals to display in the conversion string.
 * @return vector of QStrings with the guarded probabilities.
 */
std::vector<QString> Utils::safeguard_probability(Eigen::VectorXf values, int precision) {
    std::vector<QString> v;

    for (int i = 0; i < values.size(); ++i) {
        v.push_back(Utils::safeguard_probability(values(i), precision));
    }
    return v;
}

/**
 * Safeguard probability conversion from float to string to never display 'inf'. Instead displays >1e12.
 *
 * @param values: vector of probability to be converted to a string.
 * @param int: the number of decimals to display in the conversion string.
 * @return vector of QStrings with the guarded probabilities.
 */
std::vector<QString> Utils::safeguard_inf(Eigen::VectorXf values, int precision) {
    std::vector<QString> v;

    for (int i = 0; i < values.size(); ++i) {
        v.push_back(Utils::safeguard_inf(values(i), precision));
    }
    return v;
}
