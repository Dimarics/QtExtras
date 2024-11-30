#ifndef TRIANGLE_3D_H
#define TRIANGLE_3D_H

#include <QVector3D>

class Triangle3D
{
public:
    Triangle3D();
    Triangle3D(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3);

    void setV1(const QVector3D &v);
    void setV1(float x, float y, float z);
    void setV2(const QVector3D &v);
    void setV2(float x, float y, float z);
    void setV3(const QVector3D &v);
    void setV3(float x, float y, float z);
    void transform(const QMatrix4x4 &transformMatrix);

    bool intersect(const Triangle3D &triangle) const;
    float rayIntersection(const QVector3D& orig, const QVector3D& dir) const;
    qreal square() const;
    QVector3D v1() const;
    QVector3D v2() const;
    QVector3D v3() const;
    QVector3D &operator[](int index);
    QVector3D operator[](int index) const;

private:
    QVector3D v[3];
};

#endif // TRIANGLE_3D_H
