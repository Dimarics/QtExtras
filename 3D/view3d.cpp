#include "view3d.h"
//#include "cameracontroller.h"
#include "functions3d.h"
#include "datastream3d.h"

#include <QMouseEvent>
#include <QFile>

//#include <Qt3DCore/QCoreSettings>
#include <Qt3DCore/QGeometry>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>

#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QRayCaster>

View3D::View3D(QWidget *parent) : QWindowContainer(new Qt3DExtras::Qt3DWindow, parent),
    m_view(qobject_cast<Qt3DExtras::Qt3DWindow*>(containedWindow())),
    m_rootEntity(new Qt3DCore::QEntity),
    m_activeJoint(nullptr)
{
    m_view->installEventFilter(this);
    m_view->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    m_view->defaultFrameGraph()->setClearColor(QColor(33, 40, 48));
    m_view->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::PickMethod::TrianglePicking);
    //m_view->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::PrimitivePicking);
    //m_view->renderSettings()->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
    m_view->setRootEntity(m_rootEntity);
    //Qt3DCore::QCoreSettings *settings = new Qt3DCore::QCoreSettings;
    //settings->setBoundingVolumesEnabled(false);
    //m_rootEntity->addComponent(settings);

    // Настройки камеры
    m_view->camera()->setViewCenter(QVector3D(0, 0, 0));
    m_view->camera()->setPosition(QVector3D(0, 0, 1000));
    //qDebug() << m_view->camera()->transform()->matrix();
    m_view->camera()->lens()->setPerspectiveProjection(25.f, qreal(m_view->width()) / qreal(m_view->height()), 1.f, 100000.f);
    //m_view->camera()->setFarPlane(3.40282e+38);

    // Свет
    Qt3DRender::QDirectionalLight *light = new Qt3DRender::QDirectionalLight(m_rootEntity);
    light->setWorldDirection(QVector3D(0, 0, -1));
    light->setIntensity(1);
    m_rootEntity->addComponent(light);
    connect(m_view->camera(), &Qt3DRender::QCamera::viewVectorChanged, this, [light](const QVector3D &viewVector)
    { light->setWorldDirection(viewVector); });

    m_rayCaster = new Qt3DRender::QRayCaster(m_rootEntity);
    m_rayCaster->setRunMode(Qt3DRender::QAbstractRayCaster::SingleShot);
    m_rootEntity->addComponent(m_rayCaster);
    connect(m_rayCaster, &Qt3DRender::QRayCaster::hitsChanged,
            this, [this](const Qt3DRender::QAbstractRayCaster::Hits &hits)
    {
        if (hits.isEmpty()) return;
        Qt3DRender::QRayCasterHit hit = hits.at(0);
        for (qsizetype i = 1; i < hits.size(); ++i)
            if (hits.at(i).distance() < hit.distance()) hit = hits.at(i);
        m_pickPos = hit.worldIntersection();
        emit objectChanged(hit.entity());

        std::function<Joint*(Qt3DCore::QEntity *)> findJoint =
                [&findJoint](Qt3DCore::QEntity *entity)->Joint*
        {
            for (Qt3DCore::QComponent *component : entity->components())
                if (Joint *joint = qobject_cast<Joint*>(component))
                    return joint;
            if (Qt3DCore::QEntity *parentEntity = entity->parentEntity())
                return findJoint(parentEntity);
            return nullptr;
        };
        m_activeJoint = findJoint(hit.entity());
    });
}

View3D::~View3D()
{
    m_rootEntity->deleteLater();
}

QVector3D View3D::dirInPoint(qreal x, qreal y)
{
    QVector4D v1(2.f * x / width() - 1.f, -2.f * y / height() + 1.f, -1.f, 1.f);
    //QVector4D v2((m_view->camera()->projectionMatrix().inverted() * v1).toVector2D(), -1.f, 0.f);
    //QVector3D dir((m_view->camera()->viewMatrix().inverted() * v2).normalized());
    QVector4D v2(m_view->camera()->projectionMatrix().inverted().map(v1).toVector2D(), -1.f, 0.f);
    QVector3D dir(m_view->camera()->viewMatrix().inverted().map(v2).normalized());
    //qDebug() << dir << m_view->camera()->viewMatrix().inverted().map(v2).normalized();
    return dir;
}

QVector3D View3D::dirInPoint(const QPointF &p)
{
    return dirInPoint(p.x(), p.y());
}

QVector3D View3D::mapToWorld(qreal x, qreal y, const QVector3D &normal, const QVector3D &p0)
{
    //QVector3D pos(m_view->camera()->viewMatrix().inverted() * QVector4D(0, 0, 0, 1.f));
    return pointIntersectionWithPlane(normal, dirInPoint(x, y), m_view->camera()->position(), p0);
}

QVector3D View3D::mapToWorld(const QPointF &p, const QVector3D &normal, const QVector3D &offset)
{
    return mapToWorld(p.x(), p.y(), normal, offset);
}

QVector2D View3D::mapFromWorld(const QVector3D &v)
{
    QVector2D result(v.project(m_view->camera()->viewMatrix(),
                               m_view->camera()->projectionMatrix(),
                               QRect(0, 0, width(), height())));
    return QVector2D(result.x(), height() - result.y());
}

Qt3DCore::QEntity *View3D::rootEntity() const
{
    return m_rootEntity;
}

Qt3DCore::QEntity *View3D::entity(const QString &name) const
{
    std::function<Qt3DCore::QNode*(Qt3DCore::QNode*)> find = [&name, &find](Qt3DCore::QNode *root)->Qt3DCore::QNode*
    {
        if (root->objectName() == name) return root;
        for (Qt3DCore::QNode *node : root->childNodes())
        {
            if (!qobject_cast<Qt3DCore::QEntity*>(node)) continue;
            node = find(node);
            if (node) return node;
        }
        return nullptr;
    };
    return qobject_cast<Qt3DCore::QEntity*>(find(m_rootEntity));
}

Qt3DRender::QCamera *View3D::camera() const
{
    return m_view->camera();
}

void View3D::rayCaster(Qt3DCore::QEntity *checkEntity)
{
    if (!checkEntity->isEnabled())
        return;

    GeometryData geometryData = getGeometryData(checkEntity);
    if (geometryData.attribute)
    {
        for (uint i = 0; i < geometryData.attribute->count() / 3; ++i)
        {
            Triangle3D triangle = trianglesFromAttribute(geometryData.attribute, i, geometryData.modelMatrix);
            float distance = triangle.rayIntersection(m_pickEvent.orig, m_pickEvent.dir);
            if (distance > 0 && (m_pickEvent.distance == 0 || distance < m_pickEvent.distance))
            {
                m_pickEvent.distance = distance;
                m_pickEvent.entity = checkEntity;
            }
        }
    }
    for (Qt3DCore::QNode *node : checkEntity->childNodes())
        if (Qt3DCore::QEntity *child = qobject_cast<Qt3DCore::QEntity*>(node))
            rayCaster(child);
}

/*void View3D::drawLine(const QVector3D& p1, const QVector3D& p2, const QColor& color, Qt3DCore::QEntity *root)
{
    Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry(root);

    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float));
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = p1.x();
    *positions++ = p1.y();
    *positions++ = p1.z();
    *positions++ = p2.x();
    *positions++ = p2.y();
    *positions++ = p2.z();

    Qt3DCore::QBuffer *positionBuffer = new Qt3DCore::QBuffer(geometry);
    positionBuffer->setData(bufferBytes);

    Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute(geometry);
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(positionBuffer);
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute);

    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(uint));
    uint *indices = reinterpret_cast<uint*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    Qt3DCore::QBuffer *indexBuffer = new Qt3DCore::QBuffer(geometry);
    indexBuffer->setData(indexBytes);

    Qt3DCore::QAttribute *indexAttribute = new Qt3DCore::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setByteStride(sizeof(uint));
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute);

    // mesh
    Qt3DRender::QGeometryRenderer *line = new Qt3DRender::QGeometryRenderer(root);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    Qt3DExtras::QDiffuseSpecularMaterial *material = new Qt3DExtras::QDiffuseSpecularMaterial(root);
    material->setAmbient(color);

    // entity
    Qt3DCore::QEntity *lineEntity = new Qt3DCore::QEntity(root);
    lineEntity->addComponent(material);
    lineEntity->addComponent(line);
}*/

void View3D::saveScene(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Не удалось записать файл!";
        return;
    }
    QDataStream stream(&file);
    stream << m_view->camera() << m_rootEntity;
}

void View3D::loadScene(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Не удалось прочитать файл!";
        return;
    }
    QDataStream stream(&file);
    stream >> m_view->camera() >> m_rootEntity;
}

void View3D::setRootEntity(Qt3DCore::QEntity *entity)
{
    m_rootEntity = entity;
    m_view->setRootEntity(m_rootEntity);
}

void View3D::jointMoveEvent(Joint *joint)
{
    if (joint->entity()->objectName() == "Палец левый")
    {
        collideEntities(joint->entity(), m_rootEntity);
    }
}

bool View3D::eventFilter(QObject *target, QEvent *event)
{
    if (target == m_view)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
        {
            m_mousePos = mouseEvent->position();
            /*QVector3D a(200, 0, 100);
            QVector3D b(-300, 2000, -100);
            QVector3D c(1000, 0, 300);

            QVector3D a2(0, 0, 0);
            QVector3D b2(0, 1000, 0);
            QVector3D c2(1000, 0, 0);

            drawLine(a, b, Qt::red, m_rootEntity);
            drawLine(b, c, Qt::red, m_rootEntity);
            drawLine(c, a, Qt::red, m_rootEntity);

            drawLine(a2, b2, Qt::red, m_rootEntity);
            drawLine(b2, c2, Qt::red, m_rootEntity);
            drawLine(c2, a2, Qt::red, m_rootEntity);*/

            /*m_pickEvent.distance = 0;
            m_pickEvent.entity = nullptr;
            m_pickEvent.orig = m_view->camera()->position();
            m_pickEvent.dir = dirInPoint(m_mousePos);
            rayCaster(m_rootEntity);
            qDebug() << m_pickEvent.entity << "\n";*/

            m_rayCaster->trigger(m_view->camera()->position(), dirInPoint(m_mousePos), 100000);

            if (mouseEvent->button() == Qt::LeftButton)
            {
                m_camera.active = true;
            }
        }
            break;

        case QEvent::MouseMove:
            if (m_activeJoint)
            {
                QMatrix4x4 worldMatrix = m_activeJoint->transform()->worldMatrix();
                QVector3D worldRotationAxis = worldMatrix.mapVector(m_activeJoint->rotationAxis());
                QVector3D worldOriginPoint = worldMatrix.map(m_activeJoint->originPoint());
                QVector3D p0(pointIntersectionWithPlane(worldRotationAxis, worldRotationAxis,
                                                        worldOriginPoint, m_pickPos));

                QVector3D v1(mapToWorld(m_mousePos, worldRotationAxis, m_pickPos) - p0);
                QVector3D v2(mapToWorld(mouseEvent->position(), worldRotationAxis, m_pickPos) - p0);

                /*QVector3D v2 = m_view->camera()->position() +
                        dirInPoint(mouseEvent->position().x(), mouseEvent->position().y()) *
                        QVector3D::dotProduct(p0 - m_view->camera()->position(), worldRotationAxis) /
                        QVector3D::dotProduct(dirInPoint(mouseEvent->position().x(), mouseEvent->position().y()),
                                              worldRotationAxis);
                QVector3D v1 = m_view->camera()->position() +
                        dirInPoint(m_mousePos.x(), m_mousePos.y()) *
                        QVector3D::dotProduct(p0 - m_view->camera()->position(), worldRotationAxis) /
                        QVector3D::dotProduct(dirInPoint(m_mousePos.x(), m_mousePos.y()),
                                              worldRotationAxis);*/

                m_activeJoint->rotate(angleBetweenVectors(v1, v2, worldRotationAxis));
                jointMoveEvent(m_activeJoint);
            }
            if (m_camera.active && !m_activeJoint)
            {
                QPointF delta = (mouseEvent->position() - m_mousePos) * m_camera.lookSpeed;
                m_view->camera()->panAboutViewCenter(-delta.x(), QVector3D(0, 0, 1));
                m_view->camera()->tiltAboutViewCenter(delta.y());
            }
            m_mousePos = mouseEvent->position();
            break;

        case QEvent::MouseButtonRelease:
            m_activeJoint = nullptr;
            if (mouseEvent->button() == Qt::LeftButton)
                m_camera.active = false;
            break;

        case QEvent::Wheel:
        {
            float k = static_cast<QWheelEvent*>(event)->angleDelta().y() > 0 ? 1.25 : 0.8;
            m_view->camera()->setPosition((m_view->camera()->position() - m_view->camera()->viewCenter()) *
                                          k + m_view->camera()->viewCenter());
        }
            break;
        default:
            break;
        }
    }
    return QWindowContainer::eventFilter(target, event);
}
