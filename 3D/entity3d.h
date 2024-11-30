#ifndef ENTITY_H
#define ENTITY_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

class Entity3D : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    Entity3D(Qt3DCore::QNode *parent = nullptr);
    void setShininess(float shininess);
    void setAmbient(const QColor &ambient);
    void setDiffuse(const QColor &diffuse);
    void setSpecular(const QColor &ambient);
    void setTransparency(quint8 alpha);
    void setOriginPoint(const QVector3D &p);
    void rotate(const QQuaternion &rotation);
    void setTransform(Qt3DCore::QTransform *transform);
    void setGeometryRender(Qt3DRender::QGeometryRenderer *geometryRender);
    void setMaterial(Qt3DExtras::QDiffuseSpecularMaterial *material);

    bool hasMaterial() const;
    bool hasGeometryRender() const;
    float shininess() const;
    QColor ambient() const;
    QColor diffuse() const;
    QColor specular() const;

    friend QDataStream &operator<< (QDataStream &stream, Entity3D *entity);
    friend QDataStream &operator>> (QDataStream &stream, Entity3D *entity);

private:
    QVector3D m_originPoint;
    QVector3D m_rotateAxis;
    Qt3DCore::QEntity *m_entity;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QGeometryRenderer *m_geometryRender;
    Qt3DExtras::QDiffuseSpecularMaterial *m_material;
};

#endif // ENTITY_H
