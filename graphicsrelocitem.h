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
    void resize(qreal width, qreal height);
    void scale(qreal scale);
    void canvasScaled(qreal scale);
    void setCenterPos(const QPointF&);
    void setCenterPos(qreal x, qreal y);
    void rotate(qreal turnAngle);
    void setShiftEnable(bool state);

    bool shiftPressed() const;
    QPointF centerPos() const;
    QPointF centerScenePos() const;
    QPointF turnAroundPoint(const QPointF& point, qreal angle, const QPointF center = QPointF()) const;
    QPointF turnAroundCenter(const QPointF&, qreal angle) const;

protected:
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override;
    int type() const override;
    virtual void resizeEvent(qreal width, qreal height);
    virtual void paintEvent(QPainter*);

private:
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
    void setPlace(qreal x, qreal y);
    void setRole(const Role role);
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
    void setPlace(qreal x, qreal y);

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
