#ifndef VIEW3D_H
#define VIEW3D_H

#include "joint.h"
#include <QVector3D>
#include <QtWidgets/private/qwindowcontainer_p.h>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QRayCaster>

class View3D : public QWindowContainer
{
    Q_OBJECT
public:
    struct Camera
    {
        bool active = false;
        float lookSpeed = 0.3;
    };
    struct PickEvent
    {
        uint vertex_1;
        uint vertex_2;
        uint vertex_3;
        float distance = 0;
        QVector3D orig;
        QVector3D dir;
        Qt3DCore::QEntity *entity = nullptr;
    };
    View3D(QWidget *parent = nullptr);
    ~View3D();
    void rayCaster(Qt3DCore::QEntity *checkEntity);
    //void drawLine(const QVector3D& p1, const QVector3D& p2, const QColor& color, Qt3DCore::QEntity *root);
    void saveScene(const QString &path);
    void loadScene(const QString &path);
    void setRootEntity(Qt3DCore::QEntity *entity);
    QVector3D dirInPoint(qreal x, qreal y);
    QVector3D dirInPoint(const QPointF &p);
    QVector3D mapToWorld(qreal x, qreal y, const QVector3D &normal, const QVector3D &offset);
    QVector3D mapToWorld(const QPointF &p, const QVector3D &normal, const QVector3D &offset);
    QVector2D mapFromWorld(const QVector3D &v);
    Qt3DCore::QEntity *rootEntity() const;
    Qt3DCore::QEntity *entity(const QString &name) const;
    Qt3DRender::QCamera *camera() const;

protected:
    virtual void jointMoveEvent(Joint *joint);
    virtual void jointDropEvent(Joint *joint);
    bool eventFilter(QObject *target, QEvent *event);

private:
    Camera m_camera;
    PickEvent m_pickEvent;
    QPointF m_mousePos;
    QVector3D m_pickPos;
    Qt3DExtras::Qt3DWindow *m_view;
    Qt3DCore::QEntity *m_rootEntity;
    Qt3DRender::QRayCaster *m_rayCaster;
    Joint* m_activeJoint;

signals:
    void objectChanged(Qt3DCore::QEntity*);
};

#endif // VIEW3D_H
