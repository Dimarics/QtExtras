#ifndef VOLUME_H
#define VOLUME_H

#include <QSpinBox>
#include <QSlider>

class Volume : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int fieldWidth WRITE setFieldWidth)
    Q_PROPERTY(int fieldHeight WRITE setFieldHeight)

public:
    Volume(QWidget* = nullptr);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setValue(int value, bool = true);
    void updateValue(int value);
    void setFieldWidth(int width);
    void setFieldHeight(int height);

    int value() const;
    int minimum() const;
    int maximum() const;

protected:
    bool eventFilter(QObject*, QEvent*) override;

private:
    QSpinBox *m_spinBox;
    QSlider *m_slider;

signals:
    void valueChanged(int);
    void sendValue(int);
};

#endif // VOLUME_H
