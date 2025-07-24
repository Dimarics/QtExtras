#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

// класс поля для ввода значений
class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    DoubleSpinBox(QWidget* = nullptr);

protected:
    void wheelEvent(QWheelEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    QValidator::State validate(QString&, int&) const override;
    double valueFromText(const QString &text) const override;

public slots:
    void updateValue(double value);
};

#endif // DOUBLESPINBOX_H
