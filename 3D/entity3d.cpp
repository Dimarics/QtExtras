#include "entity3d.h"

Entity3D::Entity3D(QNode *parent) : Qt3DCore::QEntity(parent),
    m_rotateAxis(0, 0, 1),
    m_transform(new Qt3DCore::QTransform),
    m_geometryRender(nullptr),
    m_material(nullptr)
{

}

void Entity3D::setShininess(float shininess)
{
    m_material->setShininess(shininess);
}

void Entity3D::setAmbient(const QColor &ambient)
{
    m_material->setAmbient(ambient);
}

void Entity3D::setDiffuse(const QColor &diffuse)
{
    m_material->setDiffuse(diffuse);
}

void Entity3D::setSpecular(const QColor &specular)
{
    m_material->setSpecular(specular);
}

void Entity3D::setTransparency(quint8 alpha)
{
    if (alpha < 255)
    {
        m_material->setAlphaBlendingEnabled(true);
        QColor color = m_material->diffuse().value<QColor>();
        color.setAlpha(alpha);
        m_material->setDiffuse(color);
    }
    else m_material->setAlphaBlendingEnabled(false);
}

void Entity3D::setOriginPoint(const QVector3D &p)
{
    m_originPoint = p;
}

void Entity3D::rotate(const QQuaternion &rotation)
{
    m_transform->setRotation(m_transform->rotation() *= rotation);
}

void Entity3D::setTransform(Qt3DCore::QTransform *transform)
{
    if (m_transform) removeComponent(m_transform);
    m_transform = transform;
    addComponent(m_transform);
}

void Entity3D::setGeometryRender(Qt3DRender::QGeometryRenderer *geometryRender)
{
    if (m_geometryRender) removeComponent(m_geometryRender);
    m_geometryRender = geometryRender;
    addComponent(m_geometryRender);
}

void Entity3D::setMaterial(Qt3DExtras::QDiffuseSpecularMaterial *material)
{
    if (m_material) removeComponent(m_material);
    m_material = material;
    addComponent(m_material);
}

bool Entity3D::hasMaterial() const
{
    return m_material != nullptr;
}

bool Entity3D::hasGeometryRender() const
{
    return m_geometryRender != nullptr;
}

float Entity3D::shininess() const
{
    return m_material->shininess();
}

QColor Entity3D::ambient() const
{
    return m_material->ambient();
}

QColor Entity3D::diffuse() const
{
    return m_material->diffuse().value<QColor>();
}

QColor Entity3D::specular() const
{
    return m_material->specular().value<QColor>();
}

QDataStream &operator<< (QDataStream &stream, Entity3D *entity)
{
    return stream;
}

QDataStream &operator>> (QDataStream &stream, Entity3D *entity)
{
    return stream;
}
