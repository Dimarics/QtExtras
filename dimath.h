#ifndef DIMATH_H
#define DIMATH_H

#include <QtMath>
#include <QtNumeric>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QDebug>

inline qreal angleBetweenVectors(const QVector2D &v1, const QVector2D &v2)
{
    return qRadiansToDegrees(acos(QVector2D::dotProduct(v1, v2) / (v1.length() * v2.length())));
}

inline qreal angleBetweenVectors(const QVector3D &v1, const QVector3D &v2)
{
    return qRadiansToDegrees(acos(QVector3D::dotProduct(v1, v2) / (v1.length() * v2.length())));
}

inline qreal vectorProjection(const QVector3D &v1, const QVector3D &v2)
{
    //return QVector3D::dotProduct(v1, v2) / v2.length();
    return cos(acos(QVector3D::dotProduct(v1, v2) / (v1.length() * v2.length())));
}

inline QVector2D rotate(const QVector2D v, qreal angle)
{
    qreal x = v.x() * cos(angle) - v.y() * sin(angle);
    qreal y = v.x() * sin(angle) + v.y() * cos(angle);
    return QVector2D(x, y);
}

inline QVector3D projectionPlane(const QVector3D &zAxis, const QVector3D &v)
{
    qreal a = asin(zAxis.x()); // ψ
    qreal b = asin(zAxis.y()); // θ
    qreal c = asin(zAxis.z()); // ⁡φ

    /*qreal x = v.x() * (cos(a) * cos(c) - sin(a) * cos(b) * sin(c)) +
              v.y() * (-cos(a) * sin(c) - sin(a) * cos(c) * cos(c)) +
              v.z() * sin(a) * sin(b);

    qreal y = v.x() * (sin(a) * cos(c) + cos(a) * cos(b) * sin(c)) +
              v.y() * (-cos(a) * sin(b) - cos(a) * cos(c) * cos(c)) -
              v.z() * cos(a) * sin(b);

    qreal z = v.x() * sin(b) * sin(c) + v.y() * sin(b) * cos(c) + v.z() * cos(b);*/

    QMatrix4x4 mat;
    mat.rotate(qRadiansToDegrees(a), qRadiansToDegrees(b), qRadiansToDegrees(c));
    QVector3D result(QVector4D(v) * mat);
    return result;
}

inline QVector3D eulerAngles(const QVector3D &v1, const QVector3D &v2)
{
    return QQuaternion::rotationTo(v1, v2).toEulerAngles();
}

inline QVector3D pointIntersectionWithPlane(const QVector3D &normal, const QVector3D &dir,
                                            const QVector3D &rayStart, const QVector3D &p0 = QVector3D())
{
    return rayStart + dir * QVector3D::dotProduct(p0 - rayStart, normal) / QVector3D::dotProduct(dir, normal);
}

// Базовые

inline qreal heightTriangle(qreal a, qreal b, qreal c)
{
    qreal p = (a + b + c) / 2.f;
    return 2.f * sqrt(p * (p - a) * (p - b) * (p - c)) / a;
}

#endif // DIMATH_H
