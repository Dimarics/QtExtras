#include "grid.h"
#include <Qt3DCore/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

Grid::Grid(QNode *parent) : Qt3DCore::QEntity(parent),
    m_count(60),
    m_step(120),
    m_lineWidth(1),
    m_positionAttribute(new Qt3DCore::QAttribute),
    m_normalAttribute(new Qt3DCore::QAttribute),
    m_material(new Qt3DExtras::QDiffuseSpecularMaterial)
{
    Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry(this);
    m_positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    m_positionAttribute->setVertexSize(3);
    m_positionAttribute->setBuffer(new Qt3DCore::QBuffer);
    geometry->addAttribute(m_positionAttribute);

    // indeces
    //geometry->addAttribute(createIndexAttribute(vertexCount));

    // normals
    m_normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    m_normalAttribute->setVertexSize(3);
    //m_normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    m_normalAttribute->setBuffer(m_positionAttribute->buffer());
    geometry->addAttribute(m_normalAttribute);

    // mesh
    Qt3DRender::QGeometryRenderer *geometryRender = new Qt3DRender::QGeometryRenderer(this);
    geometryRender->setGeometry(geometry);
    geometryRender->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);

    // материал
    m_material->setAmbient(Qt::gray);
    m_material->setDiffuse(QColor(Qt::black));
    m_material->setSpecular(QColor(Qt::black));
    m_material->setShininess(0);

    // entity
    addComponent(m_material);
    addComponent(geometryRender);

    initBuffer();
}

int Grid::count() const { return m_count; }
float Grid::step() const { return m_step; }
QColor Grid::color() const { return m_material->ambient(); }

void Grid::initBuffer()
{
    int vertexCount = 4 * m_count;
    QByteArray bufferBytes;
    bufferBytes.resize(3 * vertexCount * sizeof(float));
    float *positions = reinterpret_cast<float*>(bufferBytes.data());

    float length = m_step * float(m_count - 1);
    float pos = -length / 2;
    //if (m_count % 2) pos += m_step;
    for (int i = 0; i < m_count; ++i)
    {
        *positions++ = pos;
        *positions++ = -length / 2;
        *positions++ = 0;

        *positions++ = pos;
        *positions++ = length / 2;
        *positions++ = 0;

        *positions++ = length / 2;
        *positions++ = pos;
        *positions++ = 0;

        *positions++ = -length / 2;
        *positions++ = pos;
        *positions++ = 0;

        pos += m_step;
    }
    m_positionAttribute->buffer()->setData(bufferBytes);

    m_positionAttribute->setCount(vertexCount);
    m_normalAttribute->setCount(vertexCount);
}

void Grid::setCount(int count)
{
    m_count = count;
    initBuffer();
}

void Grid::setStep(float step)
{
    m_step = step;
    initBuffer();
}

void Grid::setColor(const QColor &color) {  m_material->setAmbient(color); }
