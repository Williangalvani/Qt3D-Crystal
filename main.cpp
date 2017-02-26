#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>

#include <Qt3DExtras/QDiffuseMapMaterial>
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
    QVector<QVector3D> vertices;

    // Each line need two points.
    // TODO: make lines with [n-1] [n] and not with [n*2] 

    /*
    vertices.resize(1e6);
    const int s = 20;
    for (auto& v : vertices) {
        v.setX(s/2 - rand() % s);
        v.setY(s/2 - rand() % s);
        v.setZ(s/2 - rand() % s);
    }*/

    for (uint i = 0; i < 2; i++) {
        int x = qPow(-1, i);
        for (uint j = 0; j < 2; j++) {
            int y = qPow(-1, j);
            for (uint k = 0; k < 2; k++) {
                int z = qPow(-1, k);
                vertices.prepend(QVector3D( 20*x, -20*y,  20*z));
                vertices.prepend(QVector3D(-20*x, -20*y, -20*z));
                vertices.prepend(QVector3D( 20*x,  20*y, -20*z));
                vertices.prepend(QVector3D(-20*x,  20*y,  20*z));

                vertices.append(QVector3D(-20*x, -20*y, -20*z));
                vertices.append(QVector3D( 20*x, -20*y, -20*z));
                vertices.append(QVector3D(-20*x, -20*y,  20*z));
                vertices.append(QVector3D(-20*x,  20*y, -20*z));
            }
        }
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

    auto posAttribute = new Qt3DRender::QAttribute();
    posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    posAttribute->setBuffer(dataBuffer);
    posAttribute->setDataType(Qt3DRender::QAttribute::Float);
    posAttribute->setDataSize(3);
    posAttribute->setByteOffset(0);
    posAttribute->setByteStride(0);
    posAttribute->setCount(vertices.size());
    posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    geometry->addAttribute(posAttribute);

    mesh->setInstanceCount(1);
    mesh->setIndexOffset(0);
    mesh->setFirstInstance(0);
    mesh->setVertexCount(vertices.size());
    mesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
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

    auto cube = new Qt3DCore::QEntity();
    auto cubeTransform = new Qt3DCore::QTransform();

    Qt3DRender::QGeometryRenderer *indicatorMesh = lineObject();
    Qt3DRender::QMaterial *material = new Qt3DExtras::QDiffuseMapMaterial(root);

    cube->addComponent(indicatorMesh);
    cube->addComponent(material);
    cube->addComponent(cubeTransform);
    cube->setParent(root);
    cube->setEnabled(true);

    view.setRootEntity(root);
    view.show();

    return app.exec();
}
