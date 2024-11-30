#include "joint.h"

Joint::Joint(Qt3DCore::QEntity *entity) :
    m_angle(0),
    m_rotationAxis(QVector3D(0, 0, 1)),
    m_entity(entity),
    m_transform(nullptr)
{
    for (Qt3DCore::QComponent *component : entity->components())
        if (auto transform = qobject_cast<Qt3DCore::QTransform*>(component))
            m_transform = transform;
    if (!m_transform)
    {
        m_transform = new Qt3DCore::QTransform;
        m_entity->addComponent(m_transform);
    }
    m_entity->addComponent(this);
}

void Joint::setPos(const QVector3D &pos) { m_transform->setTranslation(pos + offset()); }
void Joint::setOriginPoint(const QVector3D &originPoint) { m_originPoint = originPoint; }
void Joint::setRotationAxis(const QVector3D &rotationAxis) { m_rotationAxis = rotationAxis; }

void Joint::rotate(qreal angle)
{
    m_angle += angle;
    m_angle = m_angle - 360.f * (int(m_angle) / 360);
    QVector3D translation = pos();
    m_transform->setRotation(m_transform->rotation() * QQuaternion::fromAxisAndAngle(m_rotationAxis, angle));
    m_transform->setTranslation(translation + offset());
}

void Joint::setAngle(qreal angle)
{
    m_angle = angle;
    QVector3D translation = pos();
    m_transform->setRotation(QQuaternion::fromAxisAndAngle(m_rotationAxis, angle));
    m_transform->setTranslation(translation + offset());
}

qreal Joint::angle() const { return m_angle; }

QVector3D Joint::pos() const { return m_transform->translation() - offset(); }

QVector3D Joint::originPoint() const
{
    //return m_transform->worldMatrix().mapVector(m_originPoint);
    return m_originPoint;
}

QVector3D Joint::rotationAxis() const
{
    //return m_transform->worldMatrix().mapVector(m_rotationAxis);
    return m_rotationAxis;
}

QVector3D Joint::offset() const
{
    QVector3D newOriginPoint = m_transform->matrix().mapVector(m_originPoint);
    return m_originPoint - newOriginPoint;
}

Qt3DCore::QEntity *Joint::entity() { return m_entity; }
Qt3DCore::QTransform *Joint::transform() { return m_transform; }

QDataStream &operator<<(QDataStream &stream, Joint *joint)
{ return stream << joint->m_angle << joint->m_originPoint << joint->m_rotationAxis; }

QDataStream &operator>>(QDataStream &stream, Joint *joint)
{
    qreal angle;
    QVector3D originPoint;
    QVector3D rotationAxis;

    stream >> angle >> originPoint >> rotationAxis;

    joint->setAngle(angle);
    joint->setOriginPoint(originPoint);
    joint->setRotationAxis(rotationAxis);

    return stream;
}
