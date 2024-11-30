#include "joystick.h"

#include <QPainter>
#include <QMouseEvent>

Joystick::Joystick(QWidget *parent) : QWidget(parent),
    m_borderWidth(3),
    m_borderColor(Qt::black),
    m_backgroundColor(Qt::gray),
    m_hoverColor(Qt::lightGray),
    m_textColor(Qt::black)
{
    setMouseTracking(true);
}

Joystick::Joystick(const QString &up, const QString &down, const QString &left, const QString &right, QWidget *parent) : QWidget(parent),
    m_borderWidth(3),
    m_borderColor(Qt::black),
    m_backgroundColor(Qt::gray),
    m_hoverColor(Qt::lightGray),
    m_textColor(Qt::black)
{
    setMouseTracking(true);
    setNames(up, down, left, right);
}

void Joystick::setNames(const QString &up, const QString &down, const QString &left, const QString &right)
{
    m_upButton.name = up;
    m_downButton.name = down;
    m_leftButton.name = left;
    m_rightButton.name = right;
}

void Joystick::setBorderWidth(float borderWidth)
{
    m_borderWidth = borderWidth;
}

void Joystick::setBorderColor(const QColor &color)
{
    m_borderColor = color;
}

void Joystick::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
}

void Joystick::setHoverColor(const QColor &color)
{
    m_hoverColor = color;
}

void Joystick::setTextColor(const QColor &color)
{
    m_textColor = color;
}

float Joystick::borderWidth() const
{
    return m_borderWidth;
}

QColor Joystick::borderColor() const
{
    return m_borderColor;
}

QColor Joystick::backgroundColor() const
{
    return m_backgroundColor;
}

QColor Joystick::hoverColor() const
{
    return m_hoverColor;
}

QColor Joystick::textColor() const
{
    return m_textColor;
}

void Joystick::mousePressEvent(QMouseEvent*)
{
    m_upButton.pressed = m_upButton.focus;
    m_downButton.pressed = m_downButton.focus;
    m_leftButton.pressed = m_leftButton.focus;
    m_rightButton.pressed = m_rightButton.focus;
    if (m_upButton.pressed) emit upButtonPressed();
    if (m_downButton.pressed) emit downButtonPressed();
    if (m_leftButton.pressed) emit leftButtonPressed();
    if (m_rightButton.pressed) emit rightButtonPressed();
}

void Joystick::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->position();
    m_upButton.focus = m_upButton.mask.contains(m_mousePos);
    m_downButton.focus = m_downButton.mask.contains(m_mousePos);
    m_leftButton.focus = m_leftButton.mask.contains(m_mousePos);
    m_rightButton.focus = m_rightButton.mask.contains(m_mousePos);
    repaint();
}

void Joystick::mouseReleaseEvent(QMouseEvent*)
{
    if (m_upButton.pressed)
    {
        m_upButton.pressed = false;
        emit upButtonReleased();
    }
    if (m_downButton.pressed)
    {
        m_downButton.pressed = false;
        emit downButtonReleased();
    }
    if (m_leftButton.pressed)
    {
        m_leftButton.pressed = false;
        emit leftButtonReleased();
    }
    if (m_rightButton.pressed)
    {
        m_rightButton.pressed = false;
        emit rightButtonReleased();
    }
}

void Joystick::leaveEvent(QEvent*)
{
    m_upButton.focus = false;
    m_downButton.focus = false;
    m_leftButton.focus = false;
    m_rightButton.focus = false;
    /*if (m_upButton.pressed)
    {
        m_upButton.pressed = false;
        emit upButtonReleased();
    }
    if (m_downButton.pressed)
    {
        m_downButton.pressed = false;
        emit downButtonReleased();
    }
    if (m_leftButton.pressed)
    {
        m_leftButton.pressed = false;
        emit leftButtonReleased();
    }
    if (m_rightButton.pressed)
    {
        m_rightButton.pressed = false;
        emit rightButtonReleased();
    }*/
    repaint();
}

void Joystick::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(m_borderColor, m_borderWidth));

    painter.setBrush(m_backgroundColor);
    painter.drawEllipse(width() / 4.f, height() / 4.f, width() / 2.f, height() / 2.f);
    painter.setBrush(m_upButton.focus ? m_hoverColor : m_backgroundColor);
    painter.drawPath(m_upButton.mask);
    painter.setBrush(m_downButton.focus ? m_hoverColor : m_backgroundColor);
    painter.drawPath(m_downButton.mask);
    painter.setBrush(m_leftButton.focus ? m_hoverColor : m_backgroundColor);
    painter.drawPath(m_leftButton.mask);
    painter.setBrush(m_rightButton.focus ? m_hoverColor : m_backgroundColor);
    painter.drawPath(m_rightButton.mask);

    painter.setPen(m_textColor);
    //const QSize upTextSize = painter.fontMetrics().size(Qt::TextSingleLine, m_upButton.name);
    painter.drawText(0, 0, width(), height() * 0.25, Qt::AlignCenter, m_upButton.name);
    painter.drawText(0, height() * 0.75, width(), height() * 0.25, Qt::AlignCenter, m_downButton.name);
    painter.drawText(0, 0, width() * 0.25, height(), Qt::AlignCenter, m_leftButton.name);
    painter.drawText(width() * 0.75, 0, width() * 0.25, height(), Qt::AlignCenter, m_rightButton.name);
}

void Joystick::resizeEvent(QResizeEvent*)
{
    float offset = m_borderWidth / 2.f;
    float w = width() - m_borderWidth;
    float h = height() - m_borderWidth;
    m_upButton.mask = QPainterPath();
    m_upButton.mask.arcMoveTo(offset, offset, w, h, 45);
    m_upButton.mask.arcTo(offset, offset, w, h, 45, 90);
    m_upButton.mask.arcTo(w / 4.f + offset, h / 4.f + offset, w / 2.f, h / 2.f, 135, -90);
    m_upButton.mask.arcTo(offset, offset, w, h, 45, 0);

    m_downButton.mask = QPainterPath();
    m_downButton.mask.arcMoveTo(offset, offset, w, h, 225);
    m_downButton.mask.arcTo(offset, offset, w, h, 225, 90);
    m_downButton.mask.arcTo(w / 4.f + offset, h / 4.f + offset, w / 2.f, h / 2.f, 315, -90);
    m_downButton.mask.arcTo(offset, offset, w, h, 225, 0);

    m_leftButton.mask = QPainterPath();
    m_leftButton.mask.arcMoveTo(offset, offset, w, h, 135);
    m_leftButton.mask.arcTo(offset, offset, w, h, 135, 90);
    m_leftButton.mask.arcTo(w / 4.f + offset, h / 4.f + offset, w / 2.f, h / 2.f, 225, -90);
    m_leftButton.mask.arcTo(offset, offset, w, h, 135, 0);

    m_rightButton.mask = QPainterPath();
    m_rightButton.mask.arcMoveTo(offset, offset, w, h, 315);
    m_rightButton.mask.arcTo(offset, offset, w, h, 315, 90);
    m_rightButton.mask.arcTo(w / 4.f + offset, h / 4.f + offset, w / 2.f, h / 2.f, 45, -90);
    m_rightButton.mask.arcTo(offset, offset, w, h, 315, 0);
}

