#ifndef DATASTREAM3D_H
#define DATASTREAM3D_H

#include <QDataStream>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

QDataStream &operator<<(QDataStream &stream, Qt3DCore::QEntity *entity);
QDataStream &operator<<(QDataStream &stream, Qt3DCore::QTransform *transform);
QDataStream &operator<<(QDataStream &stream, Qt3DCore::QGeometry *geometry);
QDataStream &operator<<(QDataStream &stream, Qt3DCore::QAttribute *attribute);
QDataStream &operator<<(QDataStream &stream, Qt3DCore::QBuffer *buffer);
QDataStream &operator<<(QDataStream &stream, Qt3DRender::QGeometryRenderer *geometryRender);
QDataStream &operator<<(QDataStream &stream, Qt3DRender::QCamera *camera);
QDataStream &operator<<(QDataStream &stream, Qt3DExtras::QDiffuseSpecularMaterial *material);

QDataStream &operator>>(QDataStream &stream, Qt3DCore::QEntity *entity);
QDataStream &operator>>(QDataStream &stream, Qt3DCore::QTransform *transform);
QDataStream &operator>>(QDataStream &stream, Qt3DCore::QGeometry *geometry);
QDataStream &operator>>(QDataStream &stream, Qt3DCore::QAttribute *attribute);
QDataStream &operator>>(QDataStream &stream, Qt3DCore::QBuffer *buffer);
QDataStream &operator>>(QDataStream &stream, Qt3DRender::QGeometryRenderer *geometryRender);
QDataStream &operator>>(QDataStream &stream, Qt3DRender::QCamera *camera);
QDataStream &operator>>(QDataStream &stream, Qt3DExtras::QDiffuseSpecularMaterial *material);

#endif // DATASTREAM3D_H
