#include "graphicsgloweffect.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

GraphicsGlowEffect::GraphicsGlowEffect(QObject *parent) : QGraphicsEffect(parent),
    m_strength(5),
    m_logicalBlurRadius(16),
    m_color(Qt::gray)
{
}

void GraphicsGlowEffect::setStrength(int value) { m_strength = value; }

void GraphicsGlowEffect::setBlurRadius(qreal value)
{
    m_logicalBlurRadius = value;
}

void GraphicsGlowEffect::setColor(QColor value) { m_color = value; }

int GraphicsGlowEffect::strength() const { return m_strength; }

qreal GraphicsGlowEffect::blurRadius() const { return m_logicalBlurRadius; }

QColor GraphicsGlowEffect::color() const { return m_color; }

QRectF GraphicsGlowEffect::boundingRectFor(const QRectF &rect) const
{
    int extent = qCeil(m_deviceBlurRadius);
    return QRect(rect.left() - extent, rect.top() - extent,
                 rect.width() + 2 * extent, rect.height() + 2 * extent);
}

void GraphicsGlowEffect::draw(QPainter* painter)
{
    /*QPoint offset;
    QImage srcImage = sourcePixmap(Qt::LogicalCoordinates, &offset).toImage();
    srcImage = std::move(srcImage).convertToFormat(QImage::Format_ARGB32);

    // blur
    QImage destImage(srcImage.size(), QImage::Format_ARGB32);
    destImage.setDevicePixelRatio(srcImage.devicePixelRatio());
    destImage.fill(Qt::transparent);
    QPainter blurPainter(&destImage);
    qt_blurImage(&blurPainter, srcImage, m_blurRadius * srcImage.devicePixelRatio() * 2, true, false);
    blurPainter.end();

    // colorize
    for (int y = 0; y < destImage.height(); ++y)
    {
        QRgb *line = reinterpret_cast<QRgb*>(destImage.scanLine(y));
        for (int x = 0; x < destImage.width(); ++x)
        {
            QColor color(m_color);
            float alpha = qAlpha(line[x]) / 255.f;
            //float a  = alpha;
            //for (int i = 0; i < m_strength; ++i)
            //    alpha = alpha + a * (1.f - alpha);
            alpha = pow(1.f - alpha, m_strength) * (alpha - 1.f) + 1.f;
            color.setAlphaF(alpha);
            line[x] = color.rgba();
        }
    }

    QTransform transform = painter->worldTransform();
    painter->translate(offset);
    painter->drawImage(0, 0, destImage);
    painter->setWorldTransform(transform);*/

    //------------------------------------------------------
    qreal scale = painter->worldTransform().m11();
    m_deviceBlurRadius = m_logicalBlurRadius * scale * 2.f;
    //m_deviceBlurRadius = 10;

    QPoint offset;
    sourcePixmap(Qt::LogicalCoordinates, &offset);
    QImage srcImage = sourcePixmap(Qt::DeviceCoordinates).toImage();
    srcImage = std::move(srcImage).convertToFormat(QImage::Format_ARGB32);
    srcImage.setDevicePixelRatio(scale * srcImage.devicePixelRatio());

    QImage destImage(srcImage.size(), QImage::Format_ARGB32);
    destImage.setDevicePixelRatio(srcImage.devicePixelRatio());
    destImage.fill(Qt::transparent);

    // blur
    QPainter blurPainter(&destImage);
    qt_blurImage(&blurPainter, srcImage, m_deviceBlurRadius, true, false);
    blurPainter.end();

    // colorize
    for (int y = 0; y < destImage.height(); ++y)
    {
        QRgb *line = reinterpret_cast<QRgb*>(destImage.scanLine(y));
        for (int x = 0; x < destImage.width(); ++x)
        {
            QColor color(m_color);
            float alpha = qAlpha(line[x]) / 255.f;
            //float a  = alpha;
            //for (int i = 0; i < m_strength; ++i)
            //    alpha = alpha + a * (1.f - alpha);
            alpha = pow(1.f - alpha, m_strength) * (alpha - 1.f) + 1.f;
            color.setAlphaF(alpha);
            line[x] = color.rgba();
        }
    }
    //painter->translate(-m_blurRadius * destImage.devicePixelRatio(), -m_blurRadius * destImage.devicePixelRatio());
    //painter->translate(offset);
    painter->drawImage(offset + QPointF(1, 1), destImage);

    drawSource(painter);
    updateBoundingRect();
    /*if (blur.hasAlphaChannel())
    {
        //Q_ASSERT(colorize.format() == QImage::Format_ARGB32_Premultiplied);
        QPainter maskPainter(&colorize);
        maskPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        maskPainter.drawImage(0, 0, blur);
    }*/
}
