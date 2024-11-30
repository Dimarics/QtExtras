#include "datastream3d.h"
#include "joint.h"
#include <QFile>

QDataStream &operator<<(QDataStream &stream, Qt3DCore::QEntity *entity)
{
    stream << entity->objectName();

    // Компоненты
    Qt3DCore::QTransform *transform = nullptr;
    Qt3DRender::QGeometryRenderer *geometryRender = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial *material = nullptr;
    Joint *joint = nullptr;

    for (Qt3DCore::QComponent *component : entity->components())
    {
        if (auto tr = qobject_cast<Qt3DCore::QTransform*>(component))
            transform = tr;
        else if (auto gr = qobject_cast<Qt3DRender::QGeometryRenderer*>(component))
            geometryRender = gr;
        else if (auto mat = qobject_cast<Qt3DExtras::QDiffuseSpecularMaterial*>(component))
            material = mat;
        else if (auto j = qobject_cast<Joint*>(component))
            joint = j;
    }

    bool hasTransform = static_cast<bool>(transform);
    bool hasGeometryRender = static_cast<bool>(geometryRender);
    bool hasMaterial = static_cast<bool>(material);
    bool hasJoint = static_cast<bool>(joint);

    stream << hasTransform
           << hasGeometryRender
           << hasMaterial
           << hasJoint;

    if (hasTransform)
        stream << transform;
    if (hasGeometryRender)
        stream << geometryRender;
    if (hasMaterial)
        stream << material;
    if (hasJoint)
        stream << joint;

    // Наследники
    qsizetype count = 0;
    for (Qt3DCore::QNode *node : entity->childNodes())
        if (qobject_cast<Qt3DCore::QEntity*>(node))
            ++count;
    stream << count;
    for (Qt3DCore::QNode *node : entity->childNodes())
        if (Qt3DCore::QEntity *entity = qobject_cast<Qt3DCore::QEntity*>(node))
            stream << entity;

    return stream;
}

QDataStream &operator<<(QDataStream &stream, Qt3DCore::QTransform *transform)
{ return stream << transform->matrix(); }

QDataStream &operator<<(QDataStream &stream, Qt3DCore::QGeometry *geometry)
{
    qsizetype count = 0;
    for (Qt3DCore::QAttribute *attribute : geometry->attributes())
        if (attribute->attributeType() != Qt3DCore::QAttribute::IndexAttribute)
            ++count;
    stream << count;
    for (Qt3DCore::QAttribute *attribute : geometry->attributes())
        if (attribute->attributeType() != Qt3DCore::QAttribute::IndexAttribute)
            stream << attribute;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, Qt3DCore::QAttribute *attribute)
{
    return stream << attribute->name()
                  << attribute->attributeType()
                  << attribute->vertexBaseType()
                  << attribute->count()
                  << attribute->byteOffset()
                  << attribute->byteStride()
                  << attribute->vertexSize()
                  << attribute->buffer();
}

QDataStream &operator<<(QDataStream &stream, Qt3DCore::QBuffer *buffer)
{ return stream << qCompress(buffer->data()); }

QDataStream &operator<<(QDataStream &stream, Qt3DRender::QGeometryRenderer *geometryRender)
{
    return stream << geometryRender->primitiveType() << geometryRender->geometry();
    /*stream << geometryRender->primitiveType();
    for (Qt3DCore::QAttribute *attribute : geometryRender->geometry()->attributes())
        if (attribute->name() == attribute->defaultPositionAttributeName())
            stream << attribute->buffer()->data();
    return stream;*/
}

QDataStream &operator<<(QDataStream &stream, Qt3DRender::QCamera *camera)
{
    return stream << camera->viewCenter() << camera->position() << camera->upVector();
}

QDataStream &operator<<(QDataStream &stream, Qt3DExtras::QDiffuseSpecularMaterial *material)
{
    return stream << material->shininess()
                  << material->ambient()
                  << material->diffuse()
                  << material->specular();
}

//================================================

QDataStream &operator>>(QDataStream &stream, Qt3DCore::QEntity *entity)
{
    QString name;
    stream >> name;
    entity->setObjectName(name);

    // Компоненты
    bool hasTransform, hasGeometryRender, hasMaterial, hasJoint;
    stream >> hasTransform >> hasGeometryRender >> hasMaterial >> hasJoint;
    if (hasTransform)
    {
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        stream >> transform;
        entity->addComponent(transform);
    }
    if (hasGeometryRender)
    {
        Qt3DRender::QGeometryRenderer *geometryRender = new Qt3DRender::QGeometryRenderer;
        stream >> geometryRender;
        entity->addComponent(geometryRender);
    }
    if (hasMaterial)
    {
        Qt3DExtras::QDiffuseSpecularMaterial *material = new Qt3DExtras::QDiffuseSpecularMaterial;
        stream >> material;
        entity->addComponent(material);
    }
    if (hasJoint)
    {
        Joint *joint = new Joint(entity);
        stream >> joint;
    }

    // Наследники
    qsizetype count;
    stream >> count;
    for (qsizetype i = 0; i < count; ++i)
    {
        Qt3DCore::QEntity *child = new Qt3DCore::QEntity(entity);
        stream >> child;
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Qt3DCore::QTransform *transform)
{
    QMatrix4x4 matrix;
    stream >> matrix;
    transform->setMatrix(matrix);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Qt3DCore::QGeometry *geometry)
{
    //uint count = 0;
    qsizetype size;
    stream >> size;
    for (qsizetype i = 0; i < size; ++i)
    {
        Qt3DCore::QAttribute *attribute = new Qt3DCore::QAttribute;
        stream >> attribute;
        //if (attribute->name() == attribute->defaultPositionAttributeName())
        //    count = attribute->count();
        geometry->addAttribute(attribute);
    }
    //geometry->addAttribute(createIndexAttribute(count));
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Qt3DCore::QAttribute *attribute)
{
    Qt3DCore::QBuffer *buffer = new Qt3DCore::QBuffer;
    Qt3DCore::QAttribute::AttributeType attributeType;
    Qt3DCore::QAttribute::VertexBaseType vertexBaseType;
    uint count, byteOffset, byteStride, vertexSize;
    QString name;

    stream >> name
            >> attributeType
            >> vertexBaseType
            >> count
            >> byteOffset
            >> byteStride
            >> vertexSize
            >> buffer;

    attribute->setAttributeType(attributeType);
    attribute->setVertexBaseType(vertexBaseType);
    attribute->setCount(count);
    attribute->setByteOffset(byteOffset);
    attribute->setByteStride(byteStride);
    attribute->setVertexSize(vertexSize);
    attribute->setName(name);
    attribute->setBuffer(buffer);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Qt3DCore::QBuffer *buffer)
{
    QByteArray data;
    stream >> data;
    buffer->setData(qUncompress(data));
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Qt3DRender::QGeometryRenderer *geometryRender)
{
    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;
    Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry(geometryRender);
    stream >> primitiveType >> geometry;
    geometryRender->setPrimitiveType(primitiveType);
    geometryRender->setGeometry(geometry);

    /*Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;
    QByteArray rawData;
    stream >> primitiveType >> rawData;
    geometryRender->setPrimitiveType(primitiveType);
    geometryRender->setGeometry(geometryFromRawData(rawData));*/

    return stream;
}

QDataStream &operator>>(QDataStream &stream, Qt3DRender::QCamera *camera)
{
    QVector3D viewCenter, position, upVector;
    stream >> viewCenter >> position >> upVector;
    camera->setViewCenter(viewCenter);
    camera->setPosition(position);
    camera->setUpVector(upVector);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Qt3DExtras::QDiffuseSpecularMaterial *material)
{
    float shininess;
    QColor ambient;
    QVariant diffuse, specular;
    stream >> shininess
            >> ambient
            >> diffuse
            >> specular;
    material->setShininess(shininess);
    material->setAmbient(ambient);
    material->setDiffuse(diffuse);
    material->setSpecular(specular);
    material->setAlphaBlendingEnabled(diffuse.value<QColor>().alpha() < 255);
    return stream;
}
