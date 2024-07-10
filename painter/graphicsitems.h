#ifndef CONTOURIMAGE_H
#define CONTOURIMAGE_H

#include <QGraphicsRectItem>

class ContourImage;
class ResizeField;

class ContourImage : public QGraphicsRectItem
{
public:
    ContourImage(const QList<QList<QPointF>>&, const qreal, const qreal);
    void resize(const qreal, const qreal);
    //void transform(const QPointF&, ResizeField*);
    void setBlocked(const bool);
    bool isBlocked() const;
    QList<QList<QPointF>> contours() const;

protected:
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override;
    int type() const override;

private:
    QList<QList<QPointF>> m_originalContours;
    QList<QList<QPointF>> m_contours;
    qreal m_imageWidth;
    qreal m_imageHeight;
    bool m_block;
    ResizeField *m_leftTopField;
    ResizeField *m_topField;
    ResizeField *m_rightTopField;
    ResizeField *m_rightField;
    ResizeField *m_rightBottomField;
    ResizeField *m_bottomField;
    ResizeField *m_leftBottomField;
    ResizeField *m_leftField;
};

//-----------------------------------------------
class ResizeField : public QGraphicsRectItem
{
public:
    enum Type
    {
        LeftTop, Left, LeftBottom,
        RightTop, Right, RightBottom,
        Top, Bottom
    };
    ResizeField(const quint8, ContourImage* = nullptr);
    void setPlace(const qreal, const qreal);
    //QPointF place() const;

protected:
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
    int type() const override;

private:
    quint8 m_type;
    ContourImage *m_parent;
    QPointF m_mousePos;
};

#endif // CONTOURIMAGE_H
