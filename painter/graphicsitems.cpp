#include "graphicsitems.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

ContourImage::ContourImage(const QList<QList<QPointF>> &contours, const qreal width, const qreal height) :
    m_originalContours(contours), m_imageWidth(width), m_imageHeight(height), m_block(false),
    m_leftTopField(new ResizeField(ResizeField::LeftTop, this)),
    m_topField(new ResizeField(ResizeField::Top, this)),
    m_rightTopField(new ResizeField(ResizeField::RightTop, this)),
    m_rightField(new ResizeField(ResizeField::Right, this)),
    m_rightBottomField(new ResizeField(ResizeField::RightBottom, this)),
    m_bottomField(new ResizeField(ResizeField::Bottom, this)),
    m_leftBottomField(new ResizeField(ResizeField::LeftBottom, this)),
    m_leftField(new ResizeField(ResizeField::Left, this))
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    m_leftTopField->setCursor(QCursor(Qt::SizeFDiagCursor));
    m_topField->setCursor(QCursor(Qt::SizeVerCursor));
    m_rightTopField->setCursor(QCursor(Qt::SizeBDiagCursor));
    m_rightField->setCursor(QCursor(Qt::SizeHorCursor));
    m_rightBottomField->setCursor(QCursor(Qt::SizeFDiagCursor));
    m_bottomField->setCursor(QCursor(Qt::SizeVerCursor));
    m_leftBottomField->setCursor(QCursor(Qt::SizeBDiagCursor));
    m_leftField->setCursor(QCursor(Qt::SizeHorCursor));
}

void ContourImage::resize(const qreal width, const qreal height)
{
    setRect(0.0, 0.0, width, height);
    m_leftTopField->setPlace(0.0, 0.0);
    m_topField->setPlace(width / 2.0, 0.0);
    m_rightTopField->setPlace(width, 0.0);
    m_rightField->setPlace(width, height / 2.0);
    m_rightBottomField->setPlace(width, height);
    m_bottomField->setPlace(width / 2.0, height);
    m_leftBottomField->setPlace(0.0, height);
    m_leftField->setPlace(0.0, height / 2.0);

    m_contours.clear();
    for (QList<QPointF> &contour : m_originalContours)
    {
        m_contours << QList<QPointF>();
        for (QPointF &point : contour)
        {
            m_contours.last() << QPointF(point.x() * rect().width() / m_imageWidth,
                                         point.y() * rect().height() / m_imageHeight);
        }
    }
}

void ContourImage::setBlocked(const bool block)
{
    m_block = block;
    update();
}

bool ContourImage::isBlocked() const
{
    return m_block;
}

QList<QList<QPointF>> ContourImage::contours() const
{
    return m_contours;
}

void ContourImage::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(isBlocked() ? Qt::red : QColor(0, 170, 230), 0));

    for (QList<QPointF> &spline : m_contours)
    {
        painter->drawPolyline(spline);
    }

    if (isSelected())
    {
        QPen pen(QColor(0, 120, 215), 0);
        pen.setDashPattern(QList<qreal>{4.0, 4.0});
        painter->setPen(pen);
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawRect(rect());
        for (QGraphicsItem *&field : childItems()) field->show();
    }
    else for (QGraphicsItem *&field : childItems()) field->hide();
}

int ContourImage::type() const
{
    return UserType;
}

//-----------------------------------------------
ResizeField::ResizeField(const quint8 type, ContourImage *parent) : QGraphicsRectItem(parent), m_type(type), m_parent(parent)
{
    const qreal side = 12.0;
    setRect(0, 0, side, side);
    setFlag(QGraphicsItem::ItemIsFocusable);
}

void ResizeField::setPlace(const qreal x, const qreal y)
{
    setPos(x - rect().width() / 2.0, y - rect().height() / 2.0);
}

void ResizeField::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const qreal side = 4.0;
    painter->setPen(QPen(QColor(85, 85, 85), 0));
    painter->setBrush(Qt::white);
    painter->drawRect((rect().width() - side) / 2.0, (rect().width() - side) / 2.0, side, side);
}

void ResizeField::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mousePos = event->scenePos();
    switch (m_type)
    {
    case LeftTop:
        m_mousePos += QPointF(m_parent->rect().width(), m_parent->rect().height());
        break;
    case Top:
        m_mousePos.setY(m_mousePos.y() + m_parent->rect().height());
        break;
    case RightTop:
        m_mousePos += QPointF(-m_parent->rect().width(), m_parent->rect().height());
        break;
    case Right:
        m_mousePos.setX(m_mousePos.x() - m_parent->rect().width());
        break;
    case RightBottom:
        m_mousePos -= QPointF(m_parent->rect().width(), m_parent->rect().height());
        break;
    case Bottom:
        m_mousePos.setY(m_mousePos.y() - m_parent->rect().height());
        break;
    case LeftBottom:
        m_mousePos += QPointF(m_parent->rect().width(), -m_parent->rect().height());
        break;
    case Left:
        m_mousePos.setX(m_mousePos.x() + m_parent->rect().width());
        break;
    default:
        break;
    }
    parentItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
    parentItem()->setSelected(true);
    //QGraphicsRectItem::mousePressEvent(event);
}

void ResizeField::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qreal current_width = m_parent->rect().width();
    qreal current_height = m_parent->rect().height();
    qreal r1 = current_width / current_height;
    switch (m_type)
    {
    case LeftTop:
    {
        qreal r2 = (-event->scenePos().x() + m_mousePos.x()) / (-event->scenePos().y() + m_mousePos.y());
        qreal k = r1 > r2 ? (-event->scenePos().x() + m_mousePos.x()) / current_width :
                            (-event->scenePos().y() + m_mousePos.y()) / current_height;
        m_parent->setPos(m_parent->pos() - QPointF(current_width * (k - 1), current_height * (k - 1)));
        m_parent->resize(current_width * k, current_height * k);
        break;
    }
    case Top:
    {
        qreal k = (-event->scenePos().y() + m_mousePos.y()) / current_height;
        m_parent->setPos(m_parent->pos().x(), m_parent->pos().y() - current_height * (k - 1));
        m_parent->resize(current_width, current_height * k);
        break;
    }
    case RightTop:
    {
        qreal r2 = (event->scenePos().x() - m_mousePos.x()) / (-event->scenePos().y() + m_mousePos.y());
        qreal k = r1 > r2 ? (event->scenePos().x() - m_mousePos.x()) / current_width :
                            (-event->scenePos().y() + m_mousePos.y()) / current_height;
        m_parent->setPos(m_parent->pos().x(), m_parent->pos().y() - current_height * (k - 1));
        m_parent->resize(current_width * k, current_height * k);
        break;
    }
    case Right:
    {
        qreal k = (event->scenePos().x() - m_mousePos.x()) / current_width;
        m_parent->resize(current_width * k, current_height);
        //m_parent->setPos(m_mousePos.x(), m_parent->pos().y());
        break;
    }
    case RightBottom:
    {
        qreal r2 = (event->scenePos().x() - m_mousePos.x()) / (event->scenePos().y() - m_mousePos.y());
        qreal k = r1 > r2 ? (event->scenePos().x() - m_mousePos.x()) / current_width :
                            (event->scenePos().y() - m_mousePos.y()) / current_height;
        m_parent->resize(current_width * k, current_height * k);
        break;
    }
    case Bottom:
    {
        qreal k = (event->scenePos().y() - m_mousePos.y()) / current_height;
        m_parent->resize(current_width, current_height * k);
        break;
    }
    case LeftBottom:
    {
        qreal r2 = (-event->scenePos().x() + m_mousePos.x()) / (event->scenePos().y() - m_mousePos.y());
        qreal k = r1 > r2 ? (-event->scenePos().x() + m_mousePos.x()) / current_width :
                            (event->scenePos().y() - m_mousePos.y()) / current_height;
        m_parent->setPos(m_parent->pos().x() - current_width * (k - 1), m_parent->pos().y());
        m_parent->resize(current_width * k, current_height * k);
        break;
    }
    case Left:
    {
        qreal k = (-event->scenePos().x() + m_mousePos.x()) / current_width;
        m_parent->setPos(m_parent->pos().x() - current_width * (k - 1), m_parent->pos().y());
        m_parent->resize(current_width * k, current_height);
        break;
    }
    default:
        break;
    }
    //static_cast<GraphicsItem*>(parentItem())->transform(event->scenePos() - m_mousePos, this);
    QGraphicsRectItem::mouseMoveEvent(event);
}

void ResizeField::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
    parentItem()->setFlag(QGraphicsItem::ItemIsMovable);
}

int ResizeField::type() const
{
    return UserType + 1;
}
