#ifndef FUNCTIONS_3D_H
#define FUNCTIONS_3D_H

#include "triangle3d.h"

#include <QtMath>
#include <QtNumeric>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QDebug>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAttribute>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QRayCaster>

struct GeometryData
{
    QMatrix4x4 modelMatrix;
    Qt3DCore::QAttribute *attribute = nullptr;
};

bool collideEntities(Qt3DCore::QEntity *entity_1, Qt3DCore::QEntity *entity_2);

GeometryData getGeometryData(Qt3DCore::QEntity *entity);

Qt3DCore::QGeometry *geometryFromRawData(const QByteArray &rawData);

inline QVector3D eulerAngles(const QVector3D &v1, const QVector3D &v2)
{
    return QQuaternion::rotationTo(v1, v2).toEulerAngles();
}

inline QQuaternion rotation(const QVector3D &v1, const QVector3D &v2)
{
    return QQuaternion::rotationTo(v1, v2);
}

inline QVector3D pointIntersectionWithPlane(const QVector3D &normal, const QVector3D &dir,
                                            const QVector3D &orig, const QVector3D &p0)
{
    return orig + dir * QVector3D::dotProduct(p0 - orig, normal) / QVector3D::dotProduct(dir, normal);
}

inline qreal angleBetweenVectors(const QVector3D &v1, const QVector3D &v2, const QVector3D &normal)
{
    qreal angle = atan2(QVector3D::dotProduct(normal, QVector3D::crossProduct(v1, v2)), QVector3D::dotProduct(v1, v2));
    return qRadiansToDegrees(angle);
}

template<typename T> inline const T *attributeIndex(Qt3DCore::QAttribute *attribute, uint index)
{
    uint offset = index * attribute->byteStride() + attribute->byteOffset();
    const char *rawData = attribute->buffer()->data().constData() + offset;
    return reinterpret_cast<const T*>(rawData);
}

inline QVector3D vertexFromAttribute(Qt3DCore::QAttribute *attribute, uint index)
{
    const float *data = attributeIndex<float>(attribute, index);
    return QVector3D(data[0], data[1], data[2]);
}

inline Triangle3D trianglesFromAttribute(Qt3DCore::QAttribute *attribute, uint index)
{
    Triangle3D triangle;
    triangle[0] = vertexFromAttribute(attribute,   index);
    triangle[1] = vertexFromAttribute(attribute, ++index);
    triangle[2] = vertexFromAttribute(attribute, ++index);
    return triangle;
}

inline Triangle3D trianglesFromAttribute(Qt3DCore::QAttribute *attribute, uint index, QMatrix4x4 transform)
{
    Triangle3D triangle(trianglesFromAttribute(attribute, index));
    triangle.transform(transform);
    return triangle;
}

template<typename T> inline void fillIndexBuffer(Qt3DCore::QAttribute *attribute)
{
    QByteArray indexBytes;
    indexBytes.resize(attribute->count() * sizeof(T));
    T *indices = reinterpret_cast<T*>(indexBytes.data());
    for (uint i = 0; i < attribute->count(); ++i)
        indices[i] = i;
    attribute->buffer()->setData(indexBytes);
}

inline void configIndexAttribute(Qt3DCore::QAttribute *attribute)
{
    if (attribute->count() <= UCHAR_MAX)
    {
        attribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedByte);
        fillIndexBuffer<uchar>(attribute);
    }
    else if (attribute->count() <= USHRT_MAX)
    {
        attribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedShort);
        fillIndexBuffer<ushort>(attribute);
    }
    else
    {
        attribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedInt);
        fillIndexBuffer<uint>(attribute);
    }
}

inline Qt3DCore::QAttribute *createIndexAttribute(uint count)
{
    Qt3DCore::QAttribute *indexAttribute = new Qt3DCore::QAttribute;
    indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
    indexAttribute->setCount(count);
    Qt3DCore::QBuffer *indexBuffer = new Qt3DCore::QBuffer;
    indexAttribute->setBuffer(indexBuffer);
    configIndexAttribute(indexAttribute);
    return indexAttribute;
}

inline bool operator<(const QVector3D &v1, const QVector3D &v2)
{ return v1.x() < v2.x() && v1.y() < v2.y() && v1.z() < v2.z(); }

inline bool operator<=(const QVector3D &v1, const QVector3D &v2)
{ return v1.x() <= v2.x() && v1.y() <= v2.y() && v1.z() <= v2.z(); }

inline bool operator>(const QVector3D &v1, const QVector3D &v2)
{ return v1.x() > v2.x() && v1.y() > v2.y() && v1.z() > v2.z(); }

inline bool operator>=(const QVector3D &v1, const QVector3D &v2)
{ return v1.x() >= v2.x() && v1.y() >= v2.y() && v1.z() >= v2.z(); }

#endif // FUNCTIONS_3D_H
