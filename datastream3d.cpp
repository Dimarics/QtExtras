#include "datastream3d.h"
#include <QFile>

DataStream3D::DataStream3D(QIODevice *device) : QDataStream(device) {}

QDataStream &DataStream3D::operator<<(Qt3DCore::QEntity *entity)
{
    *this << entity->objectName();

    // Компоненты
    Qt3DCore::QTransform *transform = nullptr;
    Qt3DRender::QGeometryRenderer *geometryRender = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial *material = nullptr;

    for (Qt3DCore::QComponent *component : entity->components())
    {
        if (qobject_cast<Qt3DCore::QTransform*>(component))
            transform = qobject_cast<Qt3DCore::QTransform*>(component);
        else if (qobject_cast<Qt3DRender::QGeometryRenderer*>(component))
            geometryRender = qobject_cast<Qt3DRender::QGeometryRenderer*>(component);
        else if (qobject_cast<Qt3DExtras::QDiffuseSpecularMaterial*>(component))
            material = qobject_cast<Qt3DExtras::QDiffuseSpecularMaterial*>(component);
    }

    bool hasTransform = transform != nullptr;
    bool hasGeometryRender = geometryRender != nullptr;
    bool hasMaterial = material != nullptr;

    *static_cast<QDataStream*>(this)
            << hasTransform
            << hasGeometryRender
            << hasMaterial;

    if (hasTransform) *this << transform;
    if (hasGeometryRender) *this << geometryRender;
    if (hasMaterial) *this << material;

    // Наследники
    qsizetype count = 0;
    for (Qt3DCore::QNode *node : entity->childNodes())
        if (qobject_cast<Qt3DCore::QEntity*>(node))
            ++count;
    QDataStream::operator<<(count);
    for (Qt3DCore::QNode *node : entity->childNodes())
        if (qobject_cast<Qt3DCore::QEntity*>(node))
            *this << qobject_cast<Qt3DCore::QEntity*>(node);

    return *this;
}

QDataStream &DataStream3D::operator<<(Qt3DCore::QTransform *transform)
{
    *this << transform->matrix();
    return *this;
}

QDataStream &DataStream3D::operator<<(Qt3DCore::QGeometry *geometry)
{
    qsizetype count = 0;
    for (Qt3DCore::QAttribute *attribute : geometry->attributes())
        if (attribute->name() != attribute->defaultTextureCoordinateAttributeName())
            ++count;
    QDataStream::operator<<(count);
    for (Qt3DCore::QAttribute *attribute : geometry->attributes())
        if (attribute->name() != attribute->defaultTextureCoordinateAttributeName())
            *this << attribute;
    return *this;
}

QDataStream &DataStream3D::operator<<(Qt3DCore::QAttribute *attribute)
{
    *static_cast<QDataStream*>(this)
            << attribute->attributeType()
            << attribute->vertexBaseType()
            << attribute->count()
            << attribute->byteOffset()
            << attribute->byteStride()
            << attribute->vertexSize()
            << attribute->name();
    *this << attribute->buffer();
    return *this;
}

QDataStream &DataStream3D::operator<<(Qt3DCore::QBuffer *buffer)
{
    *this << qCompress(buffer->data());
    return *this;
}

QDataStream &DataStream3D::operator<<(Qt3DRender::QGeometryRenderer *geometryRender)
{
    *this << geometryRender->geometry();
    return *this;
}

QDataStream &DataStream3D::operator<<(Qt3DExtras::QDiffuseSpecularMaterial *material)
{
    *static_cast<QDataStream*>(this)
            << material->shininess()
            << material->ambient()
            << material->diffuse()
            << material->specular();
    return *this;
}

//================================================

QDataStream &DataStream3D::operator>>(Qt3DCore::QEntity *entity)
{
    QString name;
    *this >> name;
    entity->setObjectName(name);

    // Компоненты
    bool hasTransform, hasGeometryRender, hasMaterial;
    *static_cast<QDataStream*>(this) >> hasTransform >> hasGeometryRender >> hasMaterial;
    if (hasTransform)
    {
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        *this >> transform;
        entity->addComponent(transform);
    }
    if (hasGeometryRender)
    {
        Qt3DRender::QGeometryRenderer *geometryRender = new Qt3DRender::QGeometryRenderer;
        *this >> geometryRender;
        entity->addComponent(geometryRender);
    }
    if (hasMaterial)
    {
        Qt3DExtras::QDiffuseSpecularMaterial *material = new Qt3DExtras::QDiffuseSpecularMaterial;
        *this >> material;
        entity->addComponent(material);
    }

    // Наследники
    qsizetype count;
    QDataStream::operator>>(count);
    for (qsizetype i = 0; i < count; ++i)
    {
        Qt3DCore::QEntity *child = new Qt3DCore::QEntity(entity);
        *this >> child;
    }

    return *this;
}

QDataStream &DataStream3D::operator>>(Qt3DCore::QTransform *transform)
{
    QMatrix4x4 matrix;
    *this >> matrix;
    transform->setMatrix(matrix);
    return *this;
}

QDataStream &DataStream3D::operator>>(Qt3DCore::QGeometry *geometry)
{
    qsizetype size;
    QDataStream::operator>>(size);
    for (qsizetype i = 0; i < size; ++i)
    {
        Qt3DCore::QAttribute *attribute = new Qt3DCore::QAttribute;
        *this >> attribute;
        geometry->addAttribute(attribute);
        //if (attribute->name() == attribute->defaultPositionAttributeName())
        //    geometry->setBoundingVolumePositionAttribute(attribute);
    }
    return *this;
}

QDataStream &DataStream3D::operator>>(Qt3DCore::QAttribute *attribute)
{
    Qt3DCore::QBuffer *buffer = new Qt3DCore::QBuffer;
    Qt3DCore::QAttribute::AttributeType attributeType;
    Qt3DCore::QAttribute::VertexBaseType vertexBaseType;
    uint count, byteOffset, byteStride, vertexSize;
    QString name;

    *static_cast<QDataStream*>(this)
            >> attributeType
            >> vertexBaseType
            >> count
            >> byteOffset
            >> byteStride
            >> vertexSize
            >> name;
    *this >> buffer;

    attribute->setAttributeType(attributeType);
    attribute->setVertexBaseType(vertexBaseType);
    attribute->setCount(count);
    attribute->setByteOffset(byteOffset);
    attribute->setByteStride(byteStride);
    attribute->setVertexSize(vertexSize);
    attribute->setName(name);
    attribute->setBuffer(buffer);
    return *this;
}

QDataStream &DataStream3D::operator>>(Qt3DCore::QBuffer *buffer)
{
    QByteArray data;
    *this >> data;
    buffer->setData(qUncompress(data));
    return *this;
}

QDataStream &DataStream3D::operator>>(Qt3DRender::QGeometryRenderer *geometryRender)
{
    Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry(geometryRender);
    *this >> geometry;
    geometryRender->setGeometry(geometry);
    return *this;
}

QDataStream &DataStream3D::operator>>(Qt3DExtras::QDiffuseSpecularMaterial *material)
{
    float shininess;
    QColor ambient;
    QVariant diffuse, specular;

    *static_cast<QDataStream*>(this)
            >> shininess
            >> ambient
            >> diffuse
            >> specular;

    material->setShininess(shininess);
    material->setAmbient(ambient);
    material->setDiffuse(diffuse);
    material->setSpecular(specular);
    material->setAlphaBlendingEnabled(diffuse.value<QColor>().alpha() < 255);

    return *this;
}


void DataStream3D::writeDMG(const QString &path, Qt3DCore::QEntity *root)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Не удалось записать файл!";
        return;
    }
    DataStream3D stream(&file);
    qsizetype count = 0;
    for (Qt3DCore::QNode *node : root->childNodes())
        if (qobject_cast<Qt3DCore::QEntity*>(node))
            ++count;
    stream.QDataStream::operator<<(count);
    for (Qt3DCore::QNode *node : root->childNodes())
        if (qobject_cast<Qt3DCore::QEntity*>(node))
            stream << qobject_cast<Qt3DCore::QEntity*>(node);
}

void DataStream3D::readDMG(const QString &path, Qt3DCore::QEntity *root)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Не удалось прочитать файл!";
        return;
    }
    DataStream3D stream(&file);
    qsizetype count;
    stream.QDataStream::operator>>(count);
    for (qsizetype i = 0; i < count; ++i)
    {
        Qt3DCore::QEntity *child = new Qt3DCore::QEntity(root);
        stream >> child;
    }
}
