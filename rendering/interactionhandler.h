#ifndef INTERACTIONHANDLER_H
#define INTERACTIONHANDLER_H

#include <QObject>
#include <QWidget>
#include "camera.h"

// Class for handling mousePress interactions on a View3D object
class InteractionHandler : public QObject
{
    Q_OBJECT

public:
    // Plane division and evaluation of which tile is clicked is done outside of this class
    struct Plane {
        QVector3D origin;
        QVector3D size;
        QVector3D normal;
        Plane(QVector3D _origin, QVector3D _size, QVector3D _normal)
            : origin(_origin), size(_size), normal(_normal) { }
        Plane() {}
    };

    explicit InteractionHandler(Camera *cam, QObject *parent = nullptr);

    bool handleMouseRelease(QMouseEvent* mouseEvent);
    inline void addPlane(QVector3D origin, QVector3D size, QVector3D normal) {
        if (size.x() < 0 || size.y() < 0 || size.z() < 0) {
            qFatal() << "Plane size must not be negative";
            return;
        }
        planes.push_back(Plane(origin, size, normal));
    }
    inline void clearPlanes() {
        planes.clear();
    }
signals:
    void intersectedPlane(Plane plane, QVector3D rayHit);
private:
    Camera *camera;
    QList<Plane> planes;
};

#endif // INTERACTIONHANDLER_H
