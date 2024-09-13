#include "contourimage.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

ContourImage::ContourImage(const QList<QList<QPointF>> &contours) :
    m_originalWidth(0),
    m_originalHeight(0)
{
    if (!contours.isEmpty()) setContours(contours);
}

void ContourImage::setContours(const QList<QList<QPointF>> &contours)
{
    m_originalContours = contours;
    qreal min_x = contours.at(0).at(0).x();
    qreal max_x = contours.at(0).at(0).x();
    qreal min_y = contours.at(0).at(0).y();
    qreal max_y = contours.at(0).at(0).y();
    for (QList<QPointF> &contour : m_originalContours)
    {
        for (QPointF &point : contour)
        {
            if (point.x() < min_x) min_x = point.x();
            if (point.x() > max_x) max_x = point.x();
            if (point.y() < min_y) min_y = point.y();
            if (point.y() > max_y) max_y = point.y();
        }
    }

    const qreal width = max_x - min_x;
    const qreal height = max_y - min_y;
    const qreal padding = 0; //(width > height ? width : height) * 0.03;
    m_originalWidth = width + padding * 2;
    m_originalHeight += height + padding * 2;

    for (QList<QPointF> &contour : m_originalContours)
    {
        for (QPointF &point : contour)
        {
            point.setX(point.x() - min_x + padding);
            point.setY(point.y() - min_y + padding);
        }
    }
    resize(m_originalWidth, m_originalHeight);

    // Находим форму
    // -----------------------------------------
    // Переводим двухмерный массив в одномерный
    QList<QPointF> points;
    for (QList<QPointF> &contour : m_originalContours)
        for (QPointF &point : contour)
            points << point;

    // находим самую левую точку
    min_x = points.first().x();
    qreal min = 0;
    for (qsizetype i = 1; i < points.size(); i++)
    {
        qreal x = points.at(i).x();
        // Выбираем самую левую или самую нижнюю точку в случае ничьей
        if ((x < min_x) || (min_x == x && points.at(i).y() < points.at(min).y()))
            min_x = points.at(i).x(), min = i;
    }

    // Помещаем самую нижнюю точку в начало списка
    qSwap(points[0], points[min]);

    // Функция для определения направления поворота
    auto skew = [](QPointF p1, QPointF p2, QPointF p3)->qreal
    {
        QPointF v1(p2 - p1);
        QPointF v2(p3 - p2);
        return v1.x() * v2.y() - v2.x() * v1.y();
    };

    /*// сортируем вершины
    for (qsizetype i = 1; i < points.size() - 1; i++)
    {
        for (qsizetype j = i + 1; j < points.size(); j++)
        {
            if (skew(points.at(0), points.at(i), points.at(j)) < 0)
            {
                qSwap(points[i], points[j]);
            }
        }
    }

    m_originalShape << points.at(0) << points.at(1);
    int count = 0;
    for (qsizetype i = 2; i < points.size(); i++)
    {
        while (skew(m_originalShape.at(m_originalShape.size() - 2), m_originalShape.last(), points.at(i)) < 0)
        {
            m_originalShape.removeLast();
        }
        m_originalShape << points.at(i);
    }


    for (qsizetype i = 2; i < m_originalShape.size(); i++)
    {
        if (skew(m_originalShape.at(i - 2), m_originalShape.at(i - 1), points.at(i)) < 0)
        {
            //m_originalShape.removeAt(i);
            ++count;
        }
        //qDebug() << skew(m_originalShape.at(i - 2), m_originalShape.at(i - 1), points.at(i));
    }
    qDebug() << count;*/

    m_originalShape << points.at(0);
    points.removeFirst();
    points << m_originalShape.first();
    while (true)
    {
        int right = 0;
        for (qsizetype i = 1; i < points.size(); i++)
        {
            if (skew(m_originalShape.last(), points.at(right), points.at(i)) > 0)
                right = i;
        }
        if (points.at(right) == m_originalShape.first())
            break;
        else
        {
            m_originalShape << points.at(right);
            points.removeAt(right);
        }
    }
    m_originalShape << m_originalShape.first();
}

bool ContourImage::crossPaths(const QPainterPath &painterPath) const
{
    QPainterPath shape(turnAroundCenter(m_shape.constFirst(), rotation()));
    for (QPointF point : m_shape)
        shape.lineTo(turnAroundCenter(point, rotation()));
    return painterPath.contains(shape.translated(pos()));
}

QList<QList<QPointF>> ContourImage::contours() const
{
    QList<QList<QPointF>> contours;
    for (QList<QPointF> &contour : QList<QList<QPointF>>(m_contours))
    {
        contours << QList<QPointF>();
        for (QPointF &point : contour)
        {
            contours.last() << turnAroundCenter(point, rotation());
        }
    }
    return contours;
}

QList<QList<QPointF>> ContourImage::originalContours() const
{
    return m_originalContours;
}

QPainterPath ContourImage::shape() const
{
    QPainterPath path;

    path.moveTo(m_shape.first());
    for (QPointF point : m_shape)
    {
        path.lineTo(point);
    }

    return path;
}

void ContourImage::resizeEvent(qreal width, qreal height)
{
    setRect(0, 0, width, height);
    setTransformOriginPoint(width / 2.f, height / 2.f);
    m_contours.clear();
    for (QList<QPointF> &contour : m_originalContours)
    {
        m_contours << QList<QPointF>();
        for (QPointF &point : contour)
        {
            m_contours.last() << QPointF(point.x() * width / m_originalWidth,
                                         point.y() * height / m_originalHeight);
        }
    }

    m_shape.clear();
    for (QPointF &point : m_originalShape)
    {
        m_shape  << QPointF(point.x() * width / m_originalWidth,
                            point.y() * height / m_originalHeight);
    }
}

void ContourImage::paintEvent(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(isBlocked() ? Qt::red : QColor(0, 170, 230), 0));
    for (QList<QPointF> &contour : m_contours)
    {
        painter->drawPolyline(contour);
    }

    /*painter->setPen(QPen(Qt::blue, 0));
    painter->setBrush(QColor(0, 0, 255, 50));
    painter->translate(centerPos());
    painter->rotate(-rotation());
    painter->translate(-centerPos());
    painter->drawPath(shape());*/

    /*painter->setPen(Qt::green);
    painter->setBrush(QColor(0, 0, 255, 50));
    painter->drawRect(boundingRect());*/
}

QDataStream &operator<< (QDataStream &stream, const ContourImage *image)
{
    stream << image->pos();
    stream << image->rect().size();
    stream << image->rotation();
    stream << image->originalContours();
    return stream;
}

QDataStream &operator>> (QDataStream &stream, ContourImage *image)
{
    qreal rotation;
    QPointF pos;
    QSizeF size;
    QList<QList<QPointF>> contours;
    stream >> pos >> size >> rotation >> contours;
    image->setContours(contours);
    image->setPos(pos);
    image->resize(size.width(), size.height());
    image->rotate(rotation);
    return stream;
}
