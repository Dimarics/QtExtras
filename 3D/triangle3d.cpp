#include "triangle3d.h"

#include <QMatrix4x4>
#include <QDebug>

Triangle3D::Triangle3D() : v{QVector3D(), QVector3D(), QVector3D()}{}

Triangle3D::Triangle3D(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3) : v{v1, v2, v3}{}

void Triangle3D::setV1(const QVector3D &vec) { v[0] = vec; }
void Triangle3D::setV1(float x, float y, float z) { v[0] = QVector3D(x, y, z); }
void Triangle3D::setV2(const QVector3D &vec) { v[1] = vec; }
void Triangle3D::setV2(float x, float y, float z) { v[1] = QVector3D(x, y, z); }
void Triangle3D::setV3(const QVector3D &vec) { v[2] = vec; }
void Triangle3D::setV3(float x, float y, float z) { v[2] = QVector3D(x, y, z); }

void Triangle3D::transform(const QMatrix4x4 &transform)
{
    v[0] = transform.map(v[0]);
    v[1] = transform.map(v[1]);
    v[2] = transform.map(v[2]);
}

bool Triangle3D::intersect(const Triangle3D &triangle) const
{
    QVector3D ab1 = v[1] - v[0];
    QVector3D bc1 = v[2] - v[1];
    QVector3D ca1 = v[0] - v[2];

    QVector3D ab2 = triangle.v[1] - triangle.v[0];
    QVector3D bc2 = triangle.v[2] - triangle.v[1];
    QVector3D ca2 = triangle.v[0] - triangle.v[2];

    float s1 = triangle.rayIntersection(v[0], ab1.normalized());
    if (s1 > 0 && s1 <= ab1.length()) return true;
    float s2 = triangle.rayIntersection(v[1], bc1.normalized());
    if (s2 > 0 && s2 <= bc1.length()) return true;
    float s3 = triangle.rayIntersection(v[2], ca1.normalized());
    if (s3 > 0 && s3 <= ca1.length()) return true;
    float s4 = rayIntersection(triangle.v[0], ab2.normalized());
    if (s4 > 0 && s4 <= ab2.length()) return true;
    float s5 = rayIntersection(triangle.v[1], bc2.normalized());
    if (s5 > 0 && s5 <= bc2.length()) return true;
    float s6 = rayIntersection(triangle.v[2], ca2.normalized());
    if (s6 > 0 && s6 <= ca2.length()) return true;
    //qDebug() << s1 << s2 << s3 << s4 << s5 << s6;
    return false;
}

float Triangle3D::rayIntersection(const QVector3D &orig, const QVector3D &dir) const
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    QVector3D e1 = v[1] - v[0];
    QVector3D e2 = v[2] - v[0];
    // Вычисление вектора нормали к плоскости
    QVector3D pvec = QVector3D::crossProduct(dir, e2);
    float det = QVector3D::dotProduct(e1, pvec);

    // Луч параллелен плоскости
    if (det < epsilon && det > -epsilon)
        return 0;

    float inv_det = 1 / det;
    QVector3D tvec = orig - v[0];
    float u = QVector3D::dotProduct(tvec, pvec) * inv_det;
    if (u < 0 || u > 1)
        return 0;

    QVector3D qvec = QVector3D::crossProduct(tvec, e1);
    float v = QVector3D::dotProduct(dir, qvec) * inv_det;
    if (v < 0 || u + v > 1)
        return 0;

    return QVector3D::dotProduct(e2, qvec) * inv_det;
}

qreal Triangle3D::square() const
{
    float a = v1().distanceToPoint(v[1]);
    float b = v[1].distanceToPoint(v[2]);
    float c = v[2].distanceToPoint(v[0]);
    qreal p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}

QVector3D Triangle3D::v1() const { return v[0]; }
QVector3D Triangle3D::v2() const { return v[1]; }
QVector3D Triangle3D::v3() const { return v[2]; }

QVector3D &Triangle3D::operator[](int index) { return v[index]; }

QVector3D Triangle3D::operator[](int index) const { return v[index]; }

