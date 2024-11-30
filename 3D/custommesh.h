#ifndef CUSTOMMESH_H
#define CUSTOMMESH_H

#include <Qt3DRender/QGeometryRenderer>

class CustomMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
public:
    CustomMesh(Qt3DCore::QNode *parent = nullptr);
    void setVertices(const QByteArray &rawData);

private:
    Qt3DCore::QAttribute *m_positionAttribute;
    Qt3DCore::QAttribute *m_normalAttribute;
};

#endif // CUSTOMMESH_H
