#ifndef GRID_H
#define GRID_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QAttribute>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

class Grid : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    Grid(Qt3DCore::QNode *parent);
    int count() const;
    float step()const;
    QColor color() const;

private:
    int m_count;
    float m_step;
    float m_lineWidth;
    Qt3DCore::QAttribute *m_positionAttribute;
    Qt3DCore::QAttribute *m_normalAttribute;
    Qt3DExtras::QDiffuseSpecularMaterial *m_material;
    void initBuffer();

public slots:
    void setCount(int count);
    void setStep(float step);
    void setColor(const QColor &color);
};

#endif // GRID_H
