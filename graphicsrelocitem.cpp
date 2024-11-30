#include "graphicsrelocitem.h"
#include "iconengine.h"
#include <QtMath>

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

GraphicsRelocItem::GraphicsRelocItem(QGraphicsItem *parent) : QGraphicsRectItem(parent),
    m_block(false),
    m_shiftPressed(false),
    m_rotationField(new RotationField(this)),
    m_resizeFields
    {
        new ResizeField(ResizeField::LeftTop, this),
        new ResizeField(ResizeField::Top, this),
        new ResizeField(ResizeField::RightTop, this),
        new ResizeField(ResizeField::Right, this),
        new ResizeField(ResizeField::RightBottom, this),
        new ResizeField(ResizeField::Bottom, this),
        new ResizeField(ResizeField::LeftBottom, this),
        new ResizeField(ResizeField::Left, this)
    }
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    rotate(0);
}

void GraphicsRelocItem::resize(const qreal width, const qreal height)
{
    setRect(0, 0, width, height);
    setTransformOriginPoint(width / 2.f, height / 2.f);

    m_rotationField->setPlace(width / 2.f, 0);
    for (ResizeField *&field : m_resizeFields)
    {
        switch (field->role())
        {
        case ResizeField::LeftTop:
            field->setPlace(0, 0);
            break;
        case ResizeField::Top:
            field->setPlace(width / 2.f, 0);
            break;
        case ResizeField::RightTop:
            field->setPlace(width, 0);
            break;
        case ResizeField::Right:
            field->setPlace(width, height / 2.f);
            break;
        case ResizeField::RightBottom:
            field->setPlace(width, height);
            break;
        case ResizeField::Bottom:
            field->setPlace(width / 2.f, height);
            break;
        case ResizeField::LeftBottom:
            field->setPlace(0, height);
            break;
        case ResizeField::Left:
            field->setPlace(0, height / 2.f);
            break;
        default:
            break;
        }
    }
    resizeEvent(width, height);
}

void GraphicsRelocItem::scale(const qreal scale)
{
    resize(rect().width() * scale, rect().height() * scale);
}

void GraphicsRelocItem::canvasScaled(const qreal scale)
{
    for (QGraphicsItem *item : childItems())
        if (item->type() == UserType + 1)
            item->setScale(1.f / scale);
    resize(rect().width(), rect().height());
}

void GraphicsRelocItem::setBlocked(const bool block)
{
    m_block = block;
    update();
}

void GraphicsRelocItem::setCenterPos(const QPointF &point)
{
    setPos(point - centerPos());
}

void GraphicsRelocItem::setCenterPos(const qreal x, const qreal y)
{
    setCenterPos(QPointF(x, y));
}

void GraphicsRelocItem::rotate(const qreal turnAngle)
{
    static const Qt::CursorShape cursorShapes[8]
    {
        Qt::SizeFDiagCursor,
        Qt::SizeVerCursor,
        Qt::SizeBDiagCursor,
        Qt::SizeHorCursor,
        Qt::SizeFDiagCursor,
        Qt::SizeVerCursor,
        Qt::SizeBDiagCursor,
        Qt::SizeHorCursor
    };
    qreal angle = rotation() + turnAngle;
    if (m_shiftPressed) angle = 15 * (int(angle) / 15);
    if (angle < 0) angle = 360 - angle;
    else if (angle > 360) angle = angle - 360;
    setRotation(angle);

    quint8 offset = int(angle + 22.5) / 45;
    for (ResizeField *field : m_resizeFields)
    {
        if (offset == 8) offset = 0;
        field->setCursor(cursorShapes[offset]);
        ++offset;
    }
}

void GraphicsRelocItem::setShiftEnable(const bool value)
{
    m_shiftPressed = value;
}

bool GraphicsRelocItem::isBlocked() const
{
    return m_block;
}

bool GraphicsRelocItem::shiftPressed() const
{
    return m_shiftPressed;
}

QPointF GraphicsRelocItem::centerPos() const
{
    return QPointF(rect().width(), rect().height()) / 2.f;
}

QPointF GraphicsRelocItem::centerScenePos() const
{
    return pos() + centerPos();
}

QPointF GraphicsRelocItem::turnAroundPoint(const QPointF &point, const qreal angle, const QPointF center) const
{
    const qreal x0 = center.x();
    const qreal y0 = center.y();
    const qreal _angle = qDegreesToRadians(angle);
    return QPointF(x0 + (point.x() - x0) * qCos(_angle) - (point.y() - y0) * qSin(_angle),
                   y0 + (point.x() - x0) * qSin(_angle) + (point.y() - y0) * qCos(_angle));
}

QPointF GraphicsRelocItem::turnAroundCenter(const QPointF &point, const qreal angle) const
{
    const qreal x0 = centerPos().x();
    const qreal y0 = centerPos().y();
    const qreal _angle = qDegreesToRadians(angle);
    return QPointF(x0 + (point.x() - x0) * qCos(_angle) - (point.y() - y0) * qSin(_angle),
                   y0 + (point.x() - x0) * qSin(_angle) + (point.y() - y0) * qCos(_angle));
}

void GraphicsRelocItem::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    paintEvent(painter);
    if (isSelected())
    {
        QPen pen(QColor(0, 120, 215), 0);
        pen.setDashPattern(QList<qreal>{4.0, 4.0});
        painter->setPen(pen);
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawRect(rect());
        //painter->drawLine(width / 2.f, 0, width / 2.f, m_rotationField->pos().y());
        for (QGraphicsItem *&field : childItems()) field->show();
    }
    else for (QGraphicsItem *&field : childItems()) field->hide();
}

void GraphicsRelocItem::resizeEvent(qreal, qreal){}

void GraphicsRelocItem::paintEvent(QPainter*){}

int GraphicsRelocItem::type() const
{
    return RelocItem;
}

//--------------------------------------------------------

ResizeField::ResizeField(const Role role, GraphicsRelocItem *parent) :
    QGraphicsEllipseItem(parent),
    m_parent(parent)
{
    const qreal side = 12.f;
    setRect(0, 0, side, side);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setRole(role);
}

void ResizeField::setPlace(const qreal x, const qreal y)
{
    setPos(x - rect().width() * scale() / 2.f, y - rect().height() * scale() / 2.f);
}

void ResizeField::setRole(const Role role)
{
    m_role = role;
    switch (role)
    {
    case Top:
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case Left:
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case LeftTop:
    case RightBottom:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case RightTop:
    case LeftBottom:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        break;
    }
}

void ResizeField::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const qreal side = 6.f;
    painter->setPen(QPen(QColor(85, 85, 85), 0));
    painter->setBrush(Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawEllipse((rect().width() - side) / 2.f, (rect().width() - side) / 2.f, side, side);
}

void ResizeField::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_startSize = m_parent->rect().size();
    m_startPos = event->scenePos();
    m_startCenterPos = m_parent->centerScenePos();
}

void ResizeField::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    const qint8 mat[8][4]
    {
        {1, 1, -1, -1}, // left top
        {0, 1,  0, -1}, // top
        {1, 1,  1, -1}, // right top
        {1, 0,  1,  0}, // right
        {1, 1,  1,  1}, // right bottom
        {0, 1,  0,  1}, // bottom
        {1, 1, -1,  1}, // left bottom
        {1, 0, -1,  0}, // left
    };

    QPointF delta = mapFromScene(scenePos() + event->scenePos() - m_startPos) * scale();
    delta = QPointF(delta.x() * mat[m_role][0], delta.y() * mat[m_role][1]);

    if (m_parent->shiftPressed() && m_role != ResizeField::Left && m_role != ResizeField::Top &&
            m_role != ResizeField::Right  && m_role != ResizeField::Bottom)
    {
        // if (delta.x() > 0 && delta.y() > 0)
        // else if (delta.x() > 0 && delta.y() < 0)
        // else if (delta.x() < 0 && delta.y() > 0)
        // else if (delta.x() < 0 && delta.y() < 0)
        qreal k[4] { 1, 1, 1, 1};
        switch (m_role)
        {
        case ResizeField::RightTop:
            k[0] = -1;
            k[1] = 1;
            k[2] = -1;
            k[3] = -1;
            break;
        case ResizeField::RightBottom:
            k[0] = -1;
            k[1] = -1;
            k[2] = 1;
            k[3] = 1;
            break;
        case ResizeField::LeftBottom:
            k[0] = 1;
            k[1] = -1;
            k[2] = -1;
            k[3] = -1;
            break;
        default:
            break;
        }

        if (delta.x() * k[0] / m_startSize.width() > delta.y() * k[1] / m_startSize.height())
        {
            delta.setY(delta.x() * k[2] * m_startSize.height() / m_startSize.width());
        }
        else
        {
            delta.setX(delta.y() * k[3] * m_startSize.width() / m_startSize.height());
        }
    }
    m_parent->resize(m_startSize.width() + delta.x() * mat[m_role][2],
            m_startSize.height() + delta.y() * mat[m_role][3]);
    m_parent->setCenterPos(m_startCenterPos + (mapToScene(delta) - scenePos()) / 2.f / scale());

    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void ResizeField::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
    //parentItem()->setFlag(QGraphicsItem::ItemIsMovable);
}

int ResizeField::type() const
{
    return GraphicsRelocItem::RelocField;
}

ResizeField::Role ResizeField::role() const
{
    return m_role;
}

//--------------------------------------------------------

RotationField::RotationField(GraphicsRelocItem *parent) : QGraphicsEllipseItem(parent),
    m_startAngle(parent->rotation()),
    m_parent(parent),
    m_line(new QGraphicsLineItem(this))
{
    const qreal side = 22.f;
    setRect(0, 0, side, side);
    setFlag(QGraphicsItem::ItemIsFocusable);
    //setPen(QPen(QColor(85, 85, 85), 0));
    //setBrush(Qt::white);
    m_line->setLine(side / 2.f, side - 3, side / 2.f, side + 10.f);
    m_line->setPen(QPen(QColor(0, 120, 215), 0));
}

void RotationField::setPlace(const qreal x, const qreal y)
{
    setPos(x - rect().width() * scale() / 2.f, y - m_line->line().y2() * scale());
}

void RotationField::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(QPen(QColor(85, 85, 85), 0));
    painter->setBrush(Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(IconEngine(IconEngine::TurnArrow, rect().width(), rect().height()).painterPath());
}

void RotationField::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mousePos = event->pos();
    //parentItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
    setCursor(Qt::ClosedHandCursor);
}

void RotationField::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    /*const QPointF p1(mapToParent(m_mousePos));
    const QPointF p2(mapToParent(event->pos()));
    const QPointF p0(m_parent->rect().width() / 2.f, m_parent->rect().height() / 2.f);
    const QVector2D v1 = QVector2D(p1 - p0).normalized();
    const QVector2D v2 = QVector2D(p2 - p0).normalized();
    qreal angle = 2 * qRadiansToDegrees(qAsin((v2 - v1).length() / 2));
    if (p2.x() < p1.x()) angle = -angle;
    m_parent->rotate(angle);*/

    const QPointF p0(m_parent->rect().width() / 2.f, m_parent->rect().height() / 2.f);
    const QPointF p1(mapToParent(m_mousePos) - p0);
    const QPointF p2(mapToParent(event->pos()) - p0);
    qreal angle = qRadiansToDegrees(qAtan2(p2.y(), p2.x()) - qAtan2(p1.y(), p1.x()));
    m_parent->rotate(angle);

    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void RotationField::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
    //parentItem()->setFlag(QGraphicsItem::ItemIsMovable);
    setCursor(Qt::OpenHandCursor);
}

int RotationField::type() const
{
    return GraphicsRelocItem::RelocField;
}
