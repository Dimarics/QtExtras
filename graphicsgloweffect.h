#ifndef GRAPHICSGLOWEFFECT_H
#define GRAPHICSGLOWEFFECT_H

#include <QGraphicsEffect>

class GraphicsGlowEffect : public QGraphicsEffect
{
public:
    GraphicsGlowEffect(QObject *parent = nullptr);

    QRectF boundingRectFor(const QRectF &rect) const;
    void setColor(QColor value);
    void setStrength(int value);
    void setBlurRadius(qreal value);
    QColor color() const;
    int strength() const;
    qreal blurRadius() const;

protected:
    void draw(QPainter* painter);

private:
    static QPixmap applyEffectToPixmap(QPixmap src, QGraphicsEffect *effect, int extent);
    int m_extent = 5;
    QColor m_color = QColor(255, 255, 255);
    int m_strength = 3;
    qreal m_blurRadius = 5.0;
};

#endif // GRAPHICSGLOWEFFECT_H
