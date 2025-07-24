#ifndef GRAPHICSGLOWEFFECT_H
#define GRAPHICSGLOWEFFECT_H

#include <QGraphicsEffect>

extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *painter, QImage &blurImage, qreal radius,
                                          bool quality, bool alphaOnly, int transposed = 0 );
extern Q_WIDGETS_EXPORT bool qt_scaleForTransform(const QTransform &transform, qreal *scale);

class GraphicsGlowEffect : public QGraphicsEffect
{
public:
    GraphicsGlowEffect(QObject *parent = nullptr);

    QRectF boundingRectFor(const QRectF &rect) const;
    void setColor(QColor value);
    void setStrength(int value);
    void setBlurRadius(qreal value);
    int strength() const;
    qreal blurRadius() const;
    QColor color() const;

protected:
    void draw(QPainter* painter);

private:
    int m_strength;
    qreal m_logicalBlurRadius;
    qreal m_deviceBlurRadius;
    QColor m_color;
};

#endif // GRAPHICSGLOWEFFECT_H
