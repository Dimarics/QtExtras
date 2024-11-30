#ifndef JOINT_H
#define JOINT_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

class Joint : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    Joint(Qt3DCore::QEntity *entity);
    void rotate(qreal angle);
    void setAngle(qreal angle);
    void setPos(const QVector3D &pos);
    void setOriginPoint(const QVector3D &originPoint);
    void setRotationAxis(const QVector3D &rotationAxis);
    qreal angle() const;
    QVector3D pos() const;
    QVector3D originPoint() const;
    QVector3D rotationAxis() const;
    QVector3D offset() const;
    Qt3DCore::QEntity *entity();
    Qt3DCore::QTransform *transform();

    friend QDataStream &operator<<(QDataStream &stream, Joint *joint);
    friend QDataStream &operator>>(QDataStream &stream, Joint *joint);

private:
    qreal m_angle;
    QVector3D m_originPoint;
    QVector3D m_rotationAxis;
    Qt3DCore::QEntity *m_entity;
    Qt3DCore::QTransform *m_transform;
};

#endif // JOINT_H
