#include "functions3d.h"

bool collideEntities(Qt3DCore::QEntity *entity_1, Qt3DCore::QEntity *entity_2)
{
    if (entity_1 == entity_2 || !entity_1->isEnabled() || !entity_2->isEnabled())
        return false;

    GeometryData gd1 = getGeometryData(entity_1);
    GeometryData gd2 = getGeometryData(entity_2);
    if (gd1.attribute && gd2.attribute)
    {
        for (uint i = 0; i < gd1.attribute->count() / 3; ++i)
        {
            Triangle3D triangle_1 = trianglesFromAttribute(gd1.attribute, i, gd1.modelMatrix);
            for (uint j = 0; j < gd1.attribute->count() / 3; ++j)
            {
                Triangle3D triangle_2 = trianglesFromAttribute(gd2.attribute, j, gd2.modelMatrix);
                if (triangle_1.intersect(triangle_2))
                {
                    qDebug() << entity_2;
                    return true;
                }
            }
        }
    }
    for (Qt3DCore::QNode *node : entity_1->childNodes())
        if (Qt3DCore::QEntity *child = qobject_cast<Qt3DCore::QEntity*>(node))
            if (collideEntities(child, entity_2)) return true;

    for (Qt3DCore::QNode *node : entity_2->childNodes())
        if (Qt3DCore::QEntity *child = qobject_cast<Qt3DCore::QEntity*>(node))
            if (collideEntities(entity_1, child)) return true;

    return false;
}

GeometryData getGeometryData(Qt3DCore::QEntity *entity)
{
    GeometryData geometryData;
    for (Qt3DCore::QComponent *component : entity->components())
    {
        if (auto transform = qobject_cast<Qt3DCore::QTransform*>(component))
            geometryData.modelMatrix = transform->worldMatrix();
        else if (auto geometryRender = qobject_cast<Qt3DRender::QGeometryRenderer*>(component))
        {
            if (geometryRender->primitiveType() == Qt3DRender::QGeometryRenderer::TriangleStrip)
                continue;
            for (Qt3DCore::QAttribute *attribute : geometryRender->geometry()->attributes())
                if (attribute->name() == attribute->defaultPositionAttributeName())
                    geometryData.attribute = attribute;
        }
    }
    return geometryData;
}
Qt3DCore::QGeometry *geometryFromRawData(const QByteArray &rawData)
{
    uint count = rawData.size() / sizeof(float) / 3;
    Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry;

    Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute(geometry);
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(count);
    positionAttribute->setBuffer(new Qt3DCore::QBuffer);
    positionAttribute->buffer()->setData(rawData);
    geometry->addAttribute(positionAttribute);

    Qt3DCore::QAttribute *normalAttribute = new Qt3DCore::QAttribute(geometry);
    normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
    normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    normalAttribute->setVertexSize(3);
    normalAttribute->setByteStride(3 * sizeof(float));
    normalAttribute->setCount(count);
    normalAttribute->setBuffer(new Qt3DCore::QBuffer);
    QByteArray normalBytes;
    normalBytes.resize(rawData.size());
    float *normalData = reinterpret_cast<float*>(normalBytes.data());
    const float *data = reinterpret_cast<const float*>(rawData.data());
    for (uint i = 0; i < count * 3; i += 9)
    {
        QVector3D v1(data[i+0], data[i+1], data[i+2]);
        QVector3D v2(data[i+3], data[i+4], data[i+5]);
        QVector3D v3(data[i+6], data[i+7], data[i+8]);
        QVector3D normal = QVector3D::normal(v1, v2, v3);
        normalData[i+0] = normal.x();
        normalData[i+1] = normal.y();
        normalData[i+2] = normal.z();
        normalData[i+3] = normal.x();
        normalData[i+4] = normal.y();
        normalData[i+5] = normal.z();
        normalData[i+6] = normal.x();
        normalData[i+7] = normal.y();
        normalData[i+8] = normal.z();
    }
    normalAttribute->buffer()->setData(normalBytes);
    geometry->addAttribute(normalAttribute);

    return geometry;
}
