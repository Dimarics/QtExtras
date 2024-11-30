#include "doublespinbox.h"
#include <QLineEdit>

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent), m_value(0)
{
    connect(this, &QDoubleSpinBox::valueChanged, this, [this](double value)
    {
        m_value = value;
    });
    connect(this, &QDoubleSpinBox::editingFinished, this, [this]
    {
        clearFocus();
    });
}

void DoubleSpinBox::setOriginValue(double value)
{
    m_value = value;
    blockSignals(true);
    setValue(value);
    blockSignals(false);
}

double DoubleSpinBox::originValue() const { return m_value; }

void DoubleSpinBox::wheelEvent(QWheelEvent*){}

void DoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
    blockSignals(true);
    QDoubleSpinBox::focusOutEvent(event);
    blockSignals(false);
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
