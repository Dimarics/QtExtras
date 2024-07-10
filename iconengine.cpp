#include "iconengine.h"
#include <QPainter>

IconEngine::IconEngine(Icon mark, const QSizeF &size)
{
    constructor(mark, size.width(), size.height());
}

IconEngine::IconEngine(Icon mark, const qreal width, const qreal height) : m_width(width), m_height(height)
{
    constructor(mark, width, height);
}

void IconEngine::constructor(Icon mark, const qreal width, const qreal height)
{
    m_width = width;
    m_height = height;
    switch (mark)
    {
    case Cross:
        m_painterPath = QPainterPath();
        m_painterPath.lineTo(0.9999 * width, 0.9999 * height);
        m_painterPath.moveTo(0.9999 * width, 0.0001);
        m_painterPath.lineTo(0.0001 * width, 0.9999 * height);
        break;

    case FullScreen:
        m_painterPath = QPainterPath();
        m_painterPath.addRect(0, 0, 1 * width, 1 * height);
        break;

    case WindowMode:
        m_painterPath = QPainterPath(QPointF(0.2 * width, 0.2 * height));
        m_painterPath.lineTo(0.2 * width, 0);
        m_painterPath.lineTo(width, 0 * height);
        m_painterPath.lineTo(width, 0.8 * height);
        m_painterPath.lineTo(0.8 * width, 0.8 * height);
        m_painterPath.addRect(0, 0.2 * height, 0.8 * width, 0.8 * height);
        break;

    case Hide:
        m_painterPath = QPainterPath(QPointF(0, 0.5 * height));
        m_painterPath.lineTo(width, 0.5 * height);
        break;

    case TurnArrow:
        m_painterPath = QPainterPath(QPointF(0.59223 * width, 0.667674 * height));
        m_painterPath.arcTo(0.181056 * width, 0.181056 * height, 0.637888 * width, 0.637888 * height, -61.1867, -289.60635);
        m_painterPath.lineTo(1.00 * width, 0.448969 * height);
        m_painterPath.lineTo(0.75 * width, 0.698969 * height);
        m_painterPath.lineTo(0.50 * width, 0.448969 * height);
        m_painterPath.arcTo(0.308634 * width, 0.308634 * height, 0.382733 * width, 0.382733 * height, 15.466, 283.3472);
        break;
    default:
        break;
    }
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(Qt::NoBrush));
}

void IconEngine::setPen(const QPen &pen)
{
    m_pen = pen;
}

void IconEngine::setPen(const QColor &color)
{
    m_pen = QPen(color);
}

void IconEngine::setBrush(const QBrush &brush)
{
    m_brush = brush;
}

void IconEngine::setBrush(const QColor &color)
{
    m_brush = QBrush(color);
}

QPen IconEngine::pen() const
{
    return m_pen;
}

QBrush IconEngine::brush() const
{
    return m_brush;
}

QPainterPath IconEngine::painterPath() const
{
    return m_painterPath;
}

QPixmap IconEngine::pixmap(const bool antialiasing)
{
    qreal pen_width = m_pen.widthF();
    if (pen_width == 0) pen_width = 1;

    QPixmap pixmap(m_width, m_height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    //painter.setPen(QPen(Qt::blue, 0));
    //painter.drawRect(0, 0, m_width - 1, m_height - 1);

    const qreal kx = (m_width - pen_width) / m_width;
    const qreal ky = (m_height - pen_width) / m_height;
    const qreal k = m_height > m_width ? kx : ky;

    //painter.scale(k, k);
    painter.scale(kx, ky);
    painter.translate(pen_width / 2.f / kx, pen_width / 2.f / ky);

    if (antialiasing)
        painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(m_pen);
    pen.setWidthF(pen_width / k);
    painter.setPen(pen);
    painter.setBrush(m_brush);
    painter.drawPath(m_painterPath);

    return pixmap;
}
