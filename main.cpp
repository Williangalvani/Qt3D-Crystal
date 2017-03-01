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
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/Qt3DWindow>

#include <QColor>
#include <QtMath>

Qt3DRender::QGeometryRenderer *lineObject()
{
    auto mesh = new Qt3DRender::QGeometryRenderer();
    auto geometry = new Qt3DRender::QGeometry(mesh);
    auto dataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);
    auto posAttribute = new Qt3DRender::QAttribute();

    QVector<QVector3D> vertices;
    vertices.resize(1e4);
    const int s = 20;
    for (auto& v : vertices) {
        v.setX(s/2 - rand() % s);
        v.setY(s/2 - rand() % s);
        v.setZ(s/2 - rand() % s);
    }

    QByteArray vertexBufferData;
    vertexBufferData.resize(vertices.size() * 3 * sizeof(float));
    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;
    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    dataBuffer->setData(vertexBufferData);

    posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    posAttribute->setBuffer(dataBuffer);
    posAttribute->setDataType(Qt3DRender::QAttribute::Float);
    posAttribute->setDataSize(3);
    posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    geometry->addAttribute(posAttribute);

    mesh->setInstanceCount(1);
    mesh->setIndexOffset(0);
    mesh->setFirstInstance(0);
    mesh->setVertexCount(vertices.size());
    mesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineStrip);
    mesh->setGeometry(geometry);

    return mesh;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;
    view.defaultFrameGraph()->setClearColor(QColor::fromRgbF(0.8, 0.8, 0.8, 1.0));

    auto root = new Qt3DCore::QEntity();

    auto camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 40, 40.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));

    auto camController = new Qt3DExtras::QFirstPersonCameraController(root);
    camController->setCamera(camera);

    auto lines = new Qt3DCore::QEntity();
    auto linesTransform = new Qt3DCore::QTransform();

    Qt3DRender::QGeometryRenderer *linesMesh = lineObject();
    auto material = new Qt3DExtras::QPhongMaterial(root);
    material->setAmbient(QColor(0, 120, 0));

    lines->addComponent(linesMesh);
    lines->addComponent(material);
    lines->addComponent(linesTransform);
    lines->setParent(root);

    view.setRootEntity(root);
    view.show();

    return app.exec();
}
