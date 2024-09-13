#ifndef GRAPHICSRELOCITEM_H
#define GRAPHICSRELOCITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPainterPath>
#include <QVector2D>

class ContourImage;
class RotationField;
class ResizeField;

class GraphicsRelocItem : public QGraphicsRectItem
{
public:
    enum Type
    {
        RelocItem = UserType,
        RelocField
    };
    GraphicsRelocItem(QGraphicsItem *parent = nullptr);
    void resize(const qreal, const qreal);
    void scale(const qreal scale);
    void canvasScaled(const qreal scale);
    void setBlocked(const bool);
    void setCenterPos(const QPointF&);
    void setCenterPos(const qreal x, const qreal y);
    void rotate(const qreal);
    void setShiftEnable(const bool);

    bool isBlocked() const;
    bool shiftPressed() const;
    QPointF centerPos() const;
    QPointF centerScenePos() const;
    QPointF turnAroundPoint(const QPointF& point, const qreal angle, const QPointF center = QPointF()) const;
    QPointF turnAroundCenter(const QPointF&, const qreal angle) const;

protected:
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override;
    int type() const override;
    virtual void resizeEvent(qreal width, qreal height);
    virtual void paintEvent(QPainter*);

private:
    bool m_block;
    bool m_shiftPressed;
    RotationField *m_rotationField;
    ResizeField *m_resizeFields[8];
};

//-----------------------------------------------

class ResizeField : public QGraphicsEllipseItem
{
public:
    enum Role
    {
        LeftTop,
        Top,
        RightTop,
        Right,
        RightBottom,
        Bottom,
        LeftBottom,
        Left
    };
    ResizeField(const Role type, GraphicsRelocItem *parent = nullptr);
    void setPlace(const qreal, const qreal);
    void setRole(const Role);
    Role role() const;
    //QPointF place() const;

protected:
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
    int type() const override;

private:
    Role m_role;
    QSizeF m_startSize;
    QPointF m_startPos;
    QPointF m_startCenterPos;
    GraphicsRelocItem *m_parent;
};

//-----------------------------------------------

class RotationField : public QGraphicsEllipseItem
{
public:
    RotationField(GraphicsRelocItem *parent = nullptr);
    void setPlace(const qreal, const qreal);

protected:
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
    int type() const override;

private:
    qreal m_startAngle;
    QVector2D v1;
    GraphicsRelocItem *m_parent;
    QGraphicsLineItem *m_line;
    QPainterPath m_arrow;
    QPointF m_mousePos;
};

#endif // GRAPHICSRELOCITEM_H
