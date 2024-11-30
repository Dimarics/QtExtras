#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QPushButton>
#include <QPainterPath>

class Joystick : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor hoverColor READ hoverColor WRITE setHoverColor NOTIFY hoverColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)

public:
    struct Button
    {
        bool focus = false;
        bool pressed = false;
        QString name;
        QPainterPath mask;
    };
    Joystick(QWidget *parent = nullptr);
    Joystick(const QString &up, const QString &down, const QString &left, const QString &right, QWidget *parent = nullptr);
    void setNames(const QString &up, const QString &down, const QString &left, const QString &right);
    void setBorderWidth(float borderWidth);
    void setBorderColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setHoverColor(const QColor &color);
    void setTextColor(const QColor &color);

    float borderWidth() const;
    QColor borderColor() const;
    QColor backgroundColor() const;
    QColor hoverColor() const;
    QColor textColor() const;

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void leaveEvent(QEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    bool m_pressed;
    float m_borderWidth;
    QColor m_borderColor;
    QColor m_backgroundColor;
    QColor m_hoverColor;
    QColor m_textColor;
    QPointF m_mousePos;
    Button m_upButton;
    Button m_downButton;
    Button m_leftButton;
    Button m_rightButton;

signals:
    void upButtonPressed();
    void downButtonPressed();
    void leftButtonPressed();
    void rightButtonPressed();

    void upButtonReleased();
    void downButtonReleased();
    void leftButtonReleased();
    void rightButtonReleased();

    void upButtonClicked();
    void downButtonClicked();
    void leftButtonClicked();
    void rightButtonClicked();

    void borderWidthChanged(float);
    void borderColorChanged(const QColor&);
    void backgroundColorChanged(const QColor&);
    void hoverColorChanged(const QColor&);
    void textColorChanged(const QColor&);
};
#endif // JOYSTICK_H
