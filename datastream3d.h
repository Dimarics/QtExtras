#ifndef DATASTREAM3D_H
#define DATASTREAM3D_H

#include <QDataStream>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

class DataStream3D : public QDataStream
{
public:
    DataStream3D(QIODevice *device);
    QDataStream &operator<<(Qt3DCore::QEntity *entity);
    QDataStream &operator<<(Qt3DCore::QTransform *transform);
    QDataStream &operator<<(Qt3DCore::QGeometry *geometry);
    QDataStream &operator<<(Qt3DCore::QAttribute *attribute);
    QDataStream &operator<<(Qt3DCore::QBuffer *buffer);
    QDataStream &operator<<(Qt3DRender::QGeometryRenderer *geometryRender);
    QDataStream &operator<<(Qt3DExtras::QDiffuseSpecularMaterial *material);

    QDataStream &operator>>(Qt3DCore::QEntity *entity);
    QDataStream &operator>>(Qt3DCore::QTransform *transform);
    QDataStream &operator>>(Qt3DCore::QGeometry *geometry);
    QDataStream &operator>>(Qt3DCore::QAttribute *attribute);
    QDataStream &operator>>(Qt3DCore::QBuffer *buffer);
    QDataStream &operator>>(Qt3DRender::QGeometryRenderer *geometryRender);
    QDataStream &operator>>(Qt3DExtras::QDiffuseSpecularMaterial *material);

    static void writeDMG(const QString &path, Qt3DCore::QEntity *root);
    static void readDMG(const QString &path, Qt3DCore::QEntity *root);
};

#endif // DATASTREAM3D_H
