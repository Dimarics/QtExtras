#include "custommesh.h"

CustomMesh::CustomMesh(QNode *parent) : Qt3DRender::QGeometryRenderer(parent),
    m_positionAttribute(new Qt3DCore::QAttribute),
    m_normalAttribute(new Qt3DCore::QAttribute)
{
    Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry(this);
    setGeometry(geometry);

    m_positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    m_positionAttribute->setVertexSize(3);
    m_positionAttribute->setByteStride(3 * sizeof(float));
    m_positionAttribute->setBuffer(new Qt3DCore::QBuffer);
    geometry->addAttribute(m_positionAttribute);
    //setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);

    // normals
    m_normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    m_normalAttribute->setVertexSize(3);
    m_normalAttribute->setByteStride(3 * sizeof(float));
    m_normalAttribute->setBuffer(new Qt3DCore::QBuffer);
    geometry->addAttribute(m_normalAttribute);
}

void CustomMesh::setVertices(const QByteArray &rawData)
{
    uint count = rawData.size() / sizeof(float) / 3;
    m_positionAttribute->setCount(count);
    m_normalAttribute->setCount(count);
    m_positionAttribute->buffer()->setData(rawData);

    QByteArray normalBytes;
    normalBytes.resize(rawData.size());
    float *normalData = reinterpret_cast<float*>(normalBytes.data());
    const float *data = reinterpret_cast<const float*>(rawData.data());
    //int j = 0;
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
    m_normalAttribute->buffer()->setData(normalBytes);
}
