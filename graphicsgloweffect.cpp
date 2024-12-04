#include "graphicsgloweffect.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

GraphicsGlowEffect::GraphicsGlowEffect(QObject *parent) : QGraphicsEffect(parent)
{
}

void GraphicsGlowEffect::setColor(QColor value) { m_color = value; }

void GraphicsGlowEffect::setStrength(int value) { m_strength = value; }

void GraphicsGlowEffect::setBlurRadius(qreal value)
{
    m_blurRadius = value;
    m_extent =  qCeil(value);
    updateBoundingRect();
}

QColor GraphicsGlowEffect::color() const { return m_color; }

int GraphicsGlowEffect::strength() const { return m_strength; }

qreal GraphicsGlowEffect::blurRadius() const { return m_blurRadius; }

QRectF GraphicsGlowEffect::boundingRectFor(const QRectF &rect) const
{
    return QRect(rect.left() - m_extent, rect.top() - m_extent,
                 rect.width() + 2 * m_extent, rect.height() + 2 * m_extent);
}

void GraphicsGlowEffect::draw(QPainter* painter)
{
    QPoint offset;
    QPixmap source = sourcePixmap(Qt::LogicalCoordinates, &offset);
    QPixmap glow;

    QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect;
    colorize->setColor(m_color);
    colorize->setStrength(1);
    glow = applyEffectToPixmap(source, colorize, 0);

    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(m_blurRadius);
    glow = applyEffectToPixmap(glow, blur, m_extent);

    for (int i = 0; i < m_strength; i++)
        painter->drawPixmap(offset - QPoint(m_extent, m_extent), glow);
    drawSource(painter);
}

QPixmap GraphicsGlowEffect::applyEffectToPixmap(QPixmap src, QGraphicsEffect *effect, int extent)
{
    if (src.isNull()) return QPixmap();
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(src);
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QSize size = src.size() + QSize(extent * 2, extent * 2);
    QPixmap res(size.width(), size.height());
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF(-extent, -extent, size.width(), size.height()));

    return res;
}
