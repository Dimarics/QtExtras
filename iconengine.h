#ifndef ICONENGINE_H
#define ICONENGINE_H

#include <QPen>
#include <QBrush>
#include <QPainterPath>

class IconEngine
{
public:
    enum Icon
    {
        Cross,
        FullScreen,
        WindowMode,
        Hide,
        TurnArrow
    };
    IconEngine(Icon mark, const QSizeF &size = QSizeF(1, 1));
    IconEngine(Icon mark, const qreal width = 1, const qreal height = 1);
    void setPen(const QPen&);
    void setPen(const QColor&);
    void setBrush(const QBrush&);
    void setBrush(const QColor&);
    QPen pen() const;
    QBrush brush() const;
    QPainterPath painterPath() const;
    QPixmap pixmap(const bool antialiasing = false);

private:
    qreal m_width;
    qreal m_height;
    QPen m_pen;
    QBrush m_brush;
    QPainterPath m_painterPath;
    void constructor(Icon mark, const qreal width, const qreal height);
};

#endif // ICONENGINE_H
