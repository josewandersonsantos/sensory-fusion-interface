#include "viewer3d.h"

#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DRender/QCamera>

Viewer3D::Viewer3D()
{
    view = new Qt3DExtras::Qt3DWindow();
    root = new Qt3DCore::QEntity();

    createCube();
    createAxis();
    createGrid();

    /* camera */
    auto cam = view->camera();

    cam->lens()->setPerspectiveProjection(
        45.0f,
        16.0f/9.0f,
        0.1f,
        1000.0f
        );

    cam->setPosition(QVector3D(5,5,10));
    cam->setViewCenter(QVector3D(0,0,0));

    /* mouse control */
    auto camController = new Qt3DExtras::QOrbitCameraController(root);
    camController->setCamera(cam);
    view->setRootEntity(root);
}

Qt3DExtras::Qt3DWindow* Viewer3D::window()
{
    return view;
}

void Viewer3D::createCube()
{
    Qt3DCore::QEntity *cube = new Qt3DCore::QEntity(root);

    auto *mesh = new Qt3DExtras::QCuboidMesh();

    auto *mat = new Qt3DExtras::QPhongMaterial();
    mat->setDiffuse(QColor(200,200,200));

    cubeTransform = new Qt3DCore::QTransform();

    cube->addComponent(mesh);
    cube->addComponent(mat);
    cube->addComponent(cubeTransform);
}

void Viewer3D::createAxis()
{
    auto createAxisLine = [&](QVector3D pos, QVector3D rot, QColor color)
    {
        Qt3DCore::QEntity *axis = new Qt3DCore::QEntity(root);

        auto *mesh = new Qt3DExtras::QCylinderMesh();

        mesh->setRadius(0.02);
        mesh->setLength(3);

        auto *mat = new Qt3DExtras::QPhongMaterial();

        mat->setDiffuse(color);

        auto *transform = new Qt3DCore::QTransform();

        transform->setTranslation(pos);
        transform->setRotation(QQuaternion::fromEulerAngles(rot));

        axis->addComponent(mesh);
        axis->addComponent(mat);
        axis->addComponent(transform);
    };

    /* X axis (red) */
    createAxisLine(
        QVector3D(1.5,0,0),
        QVector3D(0,0,90),
        QColor(255,0,0)
        );

    /* Y axis (green) */
    createAxisLine(
        QVector3D(0,1.5,0),
        QVector3D(0,0,0),
        QColor(0,255,0)
        );

    /* Z axis (blue) */
    createAxisLine(
        QVector3D(0,0,1.5),
        QVector3D(90,0,0),
        QColor(0,0,255)
        );
}

void Viewer3D::createGrid()
{
    Qt3DCore::QEntity *plane = new Qt3DCore::QEntity(root);

    auto *mesh = new Qt3DExtras::QPlaneMesh();

    mesh->setWidth(10);
    mesh->setHeight(10);

    auto *mat = new Qt3DExtras::QPhongMaterial();

    mat->setDiffuse(QColor(80,80,80));

    auto *transform = new Qt3DCore::QTransform();

    transform->setRotationX(-90);

    plane->addComponent(mesh);
    plane->addComponent(mat);
    plane->addComponent(transform);
}

void Viewer3D::setRotation(float roll,float pitch,float yaw)
{
    QQuaternion q = QQuaternion::fromEulerAngles(pitch, yaw, roll);
    cubeTransform->setRotation(q);
}
