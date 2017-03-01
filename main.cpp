#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/Qt3DWindow>

#include <QColor>
#include <QtMath>

#include "orbittransformcontroller.h"
#include "qorbitcameracontroller.h"


Qt3DRender::QGeometryRenderer *lineObject(QVector3D point1, QVector3D point2, float width)
{
    // math
    auto direction = (point2 - point1).normalized();

    auto offset = direction*width;

    auto extreme1 = point1 - offset;
    auto top1 = point1 + QVector3D(0, 0, width/2);
    auto bottom1 = point1 - QVector3D(0, 0, width/2);
    auto side_dir = QVector3D::crossProduct(direction, QVector3D(0,0,1));
    side_dir.normalize();
    side_dir*=width;
    auto left1 = point1 + side_dir ;
    auto right1 = point1 - side_dir;
    ///side 2
    auto extreme2 = point2 + offset;
    auto top2 = point2 + QVector3D(0, 0, width/2);
    auto bottom2 = point2 - QVector3D(0,0, width/2);
    auto left2 = point2 + side_dir;
    auto right2 = point2 - side_dir;

    auto mesh = new Qt3DRender::QGeometryRenderer();
    auto geometry = new Qt3DRender::QGeometry(mesh);
    auto vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);
    auto indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, geometry);

    // 4 distinct vertices
    QByteArray vertexBufferData;
    vertexBufferData.resize(10 * (3 + 3 + 3) * sizeof(float));

    // Vertices
    auto v0 = extreme1;
    auto v1 = top1;
    auto v2 = bottom1;
    auto v3 = left1;
    auto v4 = right1;
    auto v5 = extreme2;
    auto v6 = top2;
    auto v7 = bottom2;
    auto v8 = left2;
    auto v9 = right2;

    // Face Normals
    auto n310 = QVector3D::normal(v3, v1, v0);
    auto n140 = QVector3D::normal(v1, v4, v0);
    auto n302 = QVector3D::normal(v3, v0, v2);
    auto n204 = QVector3D::normal(v2, v0, v4);
    auto n164 = QVector3D::normal(v1, v6, v4);
    auto n694 = QVector3D::normal(v6, v9, v4);
    auto n136 = QVector3D::normal(v1, v3, v6);
    auto n863 = QVector3D::normal(v8, v6, v3);
    auto n273 = QVector3D::normal(v2, v7, v3);
    auto n783 = QVector3D::normal(v7, v8, v3);
    auto n749 = QVector3D::normal(v7, v4, v9);
    auto n472 = QVector3D::normal(v4, v7, v2);
    auto n685 = QVector3D::normal(v6, v8, v5);
    auto n965 = QVector3D::normal(v9, v6, v5);
    auto n587 = QVector3D::normal(v5, v8, v7);
    auto n579 = QVector3D::normal(v5, v7, v9);

    // Vertex Normals
    auto n0 = QVector3D(n310 + n140 + n302 + n204).normalized();
    auto n1 = QVector3D(n310 + n140 + n164 + n136).normalized();
    auto n2 = QVector3D(n302 + n204 + n273 + n472).normalized();
    auto n3 = QVector3D(n310 + n302 + n136 + n863 + n273 + n783).normalized();
    auto n4 = QVector3D(n140 + n204 + n164 + n694 + n749 + n472).normalized();
    auto n5 = QVector3D(n685 + n965 + n587 + n579).normalized();
    auto n6 = QVector3D(n164 + n694 + n136 + n863 + n685 + n965).normalized();
    auto n7 = QVector3D(n273 + n783 + n749 + n472 + n587 + n579).normalized();
    auto n8 = QVector3D(n863 + n783 + n685 + n587).normalized();
    auto n9 = QVector3D(n694 + n749 + n965 + n579).normalized();

    // Colors
    QVector3D red(0.5f, 0.0f, 0.0f);
    QVector3D green(0.0f, 0.5f, 0.0f);
    QVector3D blue(0.0f, 0.0f, 0.5f);
    QVector3D white(0.5f, 0.5f, 0.5f);

    QVector<QVector3D> vertices = QVector<QVector3D>()
            << v0 << v1 << v2 << v3 << v4 << v5 << v6 << v7 << v8 << v9
            << n0 << n1 << n2 << n3 << n4 << n5 << n6 << n7 << n8 << n9
            << red << green << green << green << green << white << blue << blue << blue << blue;

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;

    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    // Indices (12)
    QByteArray indexBufferData;
    indexBufferData.resize(16 * 3 * sizeof(ushort));
    ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

    ushort index = 0;
    // Front
    rawIndexArray[index++] = 3;
    rawIndexArray[index++] = 1;
    rawIndexArray[index++] = 0;
    // Bottom
    rawIndexArray[index++] = 1;
    rawIndexArray[index++] = 4;
    rawIndexArray[index++] = 0;
    // Left
    rawIndexArray[index++] = 3;
    rawIndexArray[index++] = 0;
    rawIndexArray[index++] = 2;

    rawIndexArray[index++] = 2;
    rawIndexArray[index++] = 0;
    rawIndexArray[index++] = 4;

    rawIndexArray[index++] = 1;
    rawIndexArray[index++] = 6;
    rawIndexArray[index++] = 4;

    rawIndexArray[index++] = 6;
    rawIndexArray[index++] = 9;
    rawIndexArray[index++] = 4;

    rawIndexArray[index++] = 1;
    rawIndexArray[index++] = 3;
    rawIndexArray[index++] = 6;

    rawIndexArray[index++] = 8;
    rawIndexArray[index++] = 6;
    rawIndexArray[index++] = 3;

    rawIndexArray[index++] = 2;
    rawIndexArray[index++] = 7;
    rawIndexArray[index++] = 3;

    rawIndexArray[index++] = 7;
    rawIndexArray[index++] = 8;
    rawIndexArray[index++] = 3;

    rawIndexArray[index++] = 7;
    rawIndexArray[index++] = 4;
    rawIndexArray[index++] = 9;

    rawIndexArray[index++] = 4;
    rawIndexArray[index++] = 7;
    rawIndexArray[index++] = 2;

    rawIndexArray[index++] = 6;
    rawIndexArray[index++] = 8;
    rawIndexArray[index++] = 5;

    rawIndexArray[index++] = 9;
    rawIndexArray[index++] = 6;
    rawIndexArray[index++] = 5;

    rawIndexArray[index++] = 5;
    rawIndexArray[index++] = 8;
    rawIndexArray[index++] = 7;

    rawIndexArray[index++] = 5;
    rawIndexArray[index++] = 7;
    rawIndexArray[index++] = 9;

    vertexDataBuffer->setData(vertexBufferData);
    indexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setCount(10);
    positionAttribute->setName(positionAttribute->defaultPositionAttributeName());

    Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
    normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(vertexDataBuffer);
    normalAttribute->setDataType(Qt3DRender::QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(10 * 3 * sizeof(float));
    normalAttribute->setByteStride(0 * sizeof(float));
    normalAttribute->setCount(10);
    normalAttribute->setName(normalAttribute->defaultNormalAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setDataType(Qt3DRender::QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(20*3 * sizeof(float));
    colorAttribute->setByteStride(0 * sizeof(float));
    colorAttribute->setCount(10);
    colorAttribute->setName(colorAttribute->defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setDataSize(3);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(16*3);

    geometry->addAttribute(positionAttribute);
    geometry->addAttribute(normalAttribute);
    geometry->addAttribute(colorAttribute);
    geometry->addAttribute(indexAttribute);

    mesh->setGeometry(geometry);

    return mesh;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;
    view.defaultFrameGraph()->setClearColor(QColor::fromRgbF(0.2, 0.2, 0.2, 1.0));

    auto root = new Qt3DCore::QEntity();

    //auto camController = new Qt3DExtras::QFirstPersonCameraController(root);
    //camController->setCamera(camera);
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;
    OrbitTransformController *controller = new OrbitTransformController(sphereTransform);
    controller->setTarget(sphereTransform);
    controller->setRadius(5.0f);
    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->lens()->setPerspectiveProjection(70.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 5.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(root);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 1800.0f );
    camController->setCamera(camera);


    auto lines = new Qt3DCore::QEntity();
    auto linesTransform = new Qt3DCore::QTransform();

    Qt3DRender::QGeometryRenderer *linesMesh = lineObject(QVector3D(-2,0,0), QVector3D(2,0,0), 0.5);
    //auto material = new Qt3DExtras::QPhongMaterial(root);
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial(root);
//
    lines->addComponent(linesMesh);
    lines->addComponent(material);
    lines->addComponent(linesTransform);
    lines->setParent(root);

    view.setRootEntity(root);
    view.show();

    return app.exec();
}
