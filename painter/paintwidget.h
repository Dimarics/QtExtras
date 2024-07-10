#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QGraphicsView>
#include <QPainterPath>

class PaintWidget : public QGraphicsView
{
    Q_OBJECT

public:
    PaintWidget(QWidget* = nullptr);
    QList<QList<QPointF>> points();

protected:
    void drawBackground(QPainter *painter, const QRectF&) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void wheelEvent(QWheelEvent*) override;

private:
    qreal m_outerCircle;
    qreal m_innerCircle;
    QPointF m_center;
    QPointF m_targetPos;
    QPainterPath m_workingArea;

public slots:
    void addImage(QList<QList<QPointF>>);
    void removeItem();
};

#endif // PAINTWIDGET_H
