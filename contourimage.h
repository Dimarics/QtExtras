#ifndef CONTOURIMAGE_H
#define CONTOURIMAGE_H

#include "graphicsrelocitem.h"
#include <QPainterPath>

class ContourImage : public GraphicsRelocItem
{
public:
    ContourImage(const QList<QList<QPointF>>& = QList<QList<QPointF>>());
    void setContours(const QList<QList<QPointF>>&);

    bool crossPaths(const QPainterPath&) const;
    QList<QList<QPointF>> contours() const;
    QList<QList<QPointF>> originalContours() const;

    QPainterPath shape() const override;

    friend QDataStream &operator<< (QDataStream &stream, const ContourImage *image);
    friend QDataStream &operator>> (QDataStream &stream, ContourImage *image);

protected:
    void resizeEvent(qreal width, qreal height) override;
    void paintEvent(QPainter*) override;

private:
    qreal m_originalWidth;
    qreal m_originalHeight;
    QList<QPointF> m_originalShape;
    QList<QPointF> m_shape;
    QList<QList<QPointF>> m_originalContours;
    QList<QList<QPointF>> m_contours;
};

#endif // CONTOURIMAGE_H
