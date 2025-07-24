#include "doublespinbox.h"
#include <QLineEdit>

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    connect(this, &QDoubleSpinBox::editingFinished, this, [this]
    {
        clearFocus();
    });
}

void DoubleSpinBox::wheelEvent(QWheelEvent*)
{
    //if (hasFocus()) clearFocus();
}

void DoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
    //blockSignals(true);
    QDoubleSpinBox::focusOutEvent(event);
    //blockSignals(false);
}

QValidator::State DoubleSpinBox::validate(QString &text, int &pos) const
{
    /*if (text.at(pos) == QChar(',') || text.at(pos) == QChar('.'))
    {
        return QValidator::Acceptable;
    }
    QValidator::State state = QDoubleSpinBox::validate(text, pos);
    return state;*/
    if (pos == 1 && (text.at(pos - 1) == ',' || text.at(pos - 1) == '.'))
        return QValidator::Invalid;
    if (pos > 0 && text.at(pos - 1) == '.')
        text[pos - 1] = ',';
    return QDoubleValidator().validate(text, pos);
}

double DoubleSpinBox::valueFromText(const QString &t) const
{
    QString text = t;
    text.replace(',', '.');
    double value = text.toDouble();
    if (value < minimum()) value = minimum();
    else if (value > maximum()) value = maximum();
    return value;
}

void DoubleSpinBox::updateValue(double value) { if (!hasFocus()) setValue(value); }
