#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

class Viewer3D
{
public:

    Viewer3D();

    Qt3DExtras::Qt3DWindow* window();
    void setRotation(float roll,float pitch,float yaw);

private:

    Qt3DExtras::Qt3DWindow *view;
    Qt3DCore::QEntity *root;

    Qt3DCore::QTransform *cubeTransform;

    void createCube();
    void createAxis();
    void createGrid();
};

#endif // VIEWER3D_H
