#include "paintwidget.h"
#include "graphicsitems.h"

#include <QKeyEvent>

PaintWidget::PaintWidget(QWidget *parent) : QGraphicsView(parent)
{
    setScene(new QGraphicsScene);
}

void PaintWidget::addImage(QList<QList<QPointF>> contours)
{
    qreal min_x = contours.at(0).at(0).x();
    qreal max_x = contours.at(0).at(0).x();
    qreal min_y = contours.at(0).at(0).y();
    qreal max_y = contours.at(0).at(0).y();
    for (QList<QPointF> &contour : contours)
    {
        for (QPointF &point : contour)
        {
            if (point.x() < min_x) min_x = point.x();
            if (point.x() > max_x) max_x = point.x();
            if (point.y() < min_y) min_y = point.y();
            if (point.y() > max_y) max_y = point.y();
        }
    }

    qreal width = max_x - min_x;
    qreal height = max_y - min_y;
    const qreal padding = (width > height ? width : height) * 0.03;

    for (QList<QPointF> &contour : contours)
    {
        for (QPointF &point : contour)
        {
            point.setX(point.x() - min_x + padding);
            point.setY(point.y() - min_y + padding);
        }
    }
    width += padding * 2;
    height += padding * 2;

    const qreal k = (m_outerCircle - m_innerCircle) / (width > height ? width : height) * 0.8;
    ContourImage *image = new ContourImage(contours, width, height);
    image->resize(width * k, height * k);
    image->setPos(-image->rect().width() / 2, -m_innerCircle - (m_outerCircle - m_innerCircle + image->rect().height()) / 2);
    scene()->addItem(image);
}

void PaintWidget::removeItem()
{
    QGraphicsItem *item = scene()->focusItem();
    if (item)
    {
        switch (item->type())
        {
        case QGraphicsItem::UserType:
            scene()->removeItem(item);
            delete item;
            break;
        case QGraphicsItem::UserType + 1:
        {
            QGraphicsItem *deleteItem = item->parentItem();
            scene()->removeItem(deleteItem);
            delete deleteItem;
            break;
        }
        default:
            break;
        }
    }
}

void PaintWidget::drawBackground(QPainter *painter, const QRectF&)
{
    const qreal fatPoint = 8.0;
    const qreal thinPoint = 6.0;
    const QColor color(125, 125, 125);

    //painter->translate(m_center);
    QFont font(painter->font());
    font.setPixelSize(12);
    painter->setFont(font);

    //pen.setDashPattern(QList<qreal>{1.0, 3.0, 3.0, 3.0});
    painter->setPen(QPen(color, 1.0, Qt::DashDotLine));
    painter->drawLine(QLineF(-m_outerCircle, 0.0, m_outerCircle, 0.0));
    painter->drawLine(QLineF(0.0, 0.0, 0.0, -m_outerCircle));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawArc(QRectF(-m_outerCircle, -m_outerCircle, m_outerCircle * 2.0, m_outerCircle * 2.0), 0, 180 * 16);
    painter->drawArc(QRectF(-m_innerCircle, -m_innerCircle, m_innerCircle * 2.0, m_innerCircle * 2.0), 0, 180 * 16);

    painter->setPen(color);
    painter->setBrush(color);

    painter->drawText(-m_innerCircle + 5.0, 12.0, "-200");
    painter->drawEllipse(QRectF(-m_innerCircle - fatPoint / 2.0, -fatPoint / 2.0, fatPoint, fatPoint));
    painter->drawText(-m_innerCircle + 4.0 - (m_outerCircle - m_innerCircle) / 2.0, 12.0, "-258");
    painter->drawEllipse(QRectF(-m_outerCircle - thinPoint / 2.0 + (m_outerCircle - m_innerCircle) / 2.0, -thinPoint / 2.0, thinPoint, thinPoint));
    painter->drawText(-m_outerCircle + 5.0, 12.0, "-315");
    painter->drawEllipse(QRectF(-m_outerCircle - fatPoint / 2.0, -fatPoint / 2.0, fatPoint, fatPoint));

    painter->drawText(5.0, -m_innerCircle + 12.0, "200");
    painter->drawEllipse(QRectF(-fatPoint / 2.0, -m_innerCircle - fatPoint / 2.0, fatPoint, fatPoint));
    painter->drawText(4.0, -m_innerCircle + 12.0 - (m_outerCircle - m_innerCircle) / 2.0, "258");
    painter->drawEllipse(QRectF(-thinPoint / 2.0, -thinPoint / 2.0 - m_innerCircle - (m_outerCircle - m_innerCircle) / 2.0, thinPoint, thinPoint));
    painter->drawText(5.0, -m_outerCircle + 12.0, "315");
    painter->drawEllipse(QRectF(-fatPoint / 2.0, -m_outerCircle - fatPoint / 2.0, fatPoint, fatPoint));

    painter->drawText(m_innerCircle - 23.0, 12.0, "200");
    painter->drawEllipse(QRectF(m_innerCircle - fatPoint / 2.0, -fatPoint / 2.0, fatPoint, fatPoint));
    painter->drawText(m_innerCircle - 22.0 + (m_outerCircle - m_innerCircle) / 2.0, 12.0, "258");
    painter->drawEllipse(QRectF(m_outerCircle - thinPoint / 2.0 - (m_outerCircle - m_innerCircle) / 2.0, -thinPoint / 2.0, thinPoint, thinPoint));
    painter->drawText(m_outerCircle - 23.0, 12.0, "315");
    painter->drawEllipse(QRectF(m_outerCircle - fatPoint / 2.0, -fatPoint / 2.0, fatPoint, fatPoint));

    for (int angle = 0; angle >= -180; angle -= 15)
    {
        const qreal x = m_outerCircle * qCos(qDegreesToRadians(angle)) - thinPoint / 2.0;
        const qreal y = m_outerCircle * qSin(qDegreesToRadians(angle)) - thinPoint / 2.0;
        const QString text = QString::number(angle + 90);
        const QSize text_size = painter->fontMetrics().size(Qt::TextSingleLine, text);
        const qreal text_x = x + text_size.width() * qCos(qDegreesToRadians(angle));
        const qreal text_y = y + text_size.height() * qSin(qDegreesToRadians(angle));

        painter->drawEllipse(QRectF(x, y, thinPoint, thinPoint));
        painter->translate(-text_size.width() / 5.0, 7.0);
        painter->drawText(text_x, text_y, text);
        painter->translate(text_size.width() / 5.0, -7.0);
    }
    painter->setPen(color);
    painter->setBrush(QColor(0, 170, 230));
    painter->drawEllipse(QRectF(-fatPoint / 2.0, -fatPoint / 2.0, fatPoint, fatPoint));
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsItem *item = scene()->focusItem();
    if (item)
    {
        switch (item->type())
        {
        case QGraphicsItem::UserType:
            static_cast<ContourImage*>(item)->setBlocked(m_workingArea.contains(item->shape().translated(item->pos())) ? false : true);
            break;
        case QGraphicsItem::UserType + 1:
            static_cast<ContourImage*>(item->parentItem())->setBlocked(m_workingArea.contains(item->parentItem()->shape().translated(item->parentItem()->pos())) ? false : true);
            break;
        default:
            break;
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void PaintWidget::resizeEvent(QResizeEvent*)
{
    static qreal old_size;
    const qreal size = qreal(height()) * 0.75 - 18.0;

    old_size = m_outerCircle;
    m_outerCircle = size > qreal(width()) / 2.0 - 40.0 ? qreal(width()) / 2.0 - 40.0 : size;
    m_innerCircle = m_outerCircle / 315.0 * 200.0;

    m_workingArea.clear();
    m_workingArea.moveTo(m_innerCircle, 0.0);
    m_workingArea.arcTo(QRectF(-m_outerCircle, -m_outerCircle, m_outerCircle * 2.0, m_outerCircle * 2.0), 0, 180);
    m_workingArea.arcTo(QRectF(-m_innerCircle, -m_innerCircle, m_innerCircle * 2.0, m_innerCircle * 2.0), 180, -180);
    m_center = QPointF(qreal(width()) / 2.0, qreal(height()) * 0.85);
    scene()->setSceneRect(-m_center.x(), -m_center.y(), width(), height());
    //m_workingArea.translate(m_center);

    for(QGraphicsItem *sceneItem : scene()->items())
    {
        if (sceneItem->type() == QGraphicsItem::UserType)
        {
            const qreal k = m_outerCircle / old_size;
            ContourImage *item = static_cast<ContourImage*>(sceneItem);
            item->resize(qreal(item->rect().width()) * k, qreal(item->rect().height()) * k);
            item->setPos(item->pos().x() * k, item->pos().y() * k);
        }
    }
}

void PaintWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
        removeItem();
    QGraphicsView::keyPressEvent(event);
}

void PaintWidget::wheelEvent(QWheelEvent*) {}

QList<QList<QPointF>> PaintWidget::points()
{
    QList<QList<QPointF>> primaryContour;
    for (QGraphicsItem *&item : scene()->items())
    {
        if (item && item->type() == QGraphicsItem::UserType && !static_cast<ContourImage*>(item)->isBlocked())
        {
            QList<QList<QPointF>> contours = static_cast<ContourImage*>(item)->contours();
            for (QList<QPointF> &contour : contours)
            {
                if (contour.size() > 1)
                {
                    qreal length = 0;
                    for (qsizetype i = 1; i < contour.size(); ++i)
                        length += (contour.at(i) - contour.at(i - 1)).manhattanLength();
                    if (length >= 0.2)
                    {
                        primaryContour << QList<QPointF>();
                        for (QPointF &point : contour)
                            primaryContour.last() << (item->pos() + point) * 315.0 / m_outerCircle;
                    }
                }
            }
        }
    }
    if (primaryContour.size() == 0) return primaryContour;

    QList<QList<QPointF>> secondaryContour;
    auto findNearest = [&](const QPointF &lastPoint)
    {
        qsizetype index = 0;
        qreal length = (primaryContour.constFirst().constFirst() - lastPoint).manhattanLength();
        for (qsizetype i = 1; i < primaryContour.size(); ++i)
        {
            qreal newLength = (primaryContour.at(i).constFirst() - lastPoint).manhattanLength();
            if (newLength < length)
            {
                index = i;
                length = newLength;
            }
        }
        secondaryContour << primaryContour.at(index);
        primaryContour.remove(index);
    };
    findNearest(QPointF(0, 0));
    while (primaryContour.size() != 0) findNearest(secondaryContour.constLast().constLast());

    return secondaryContour;
}
