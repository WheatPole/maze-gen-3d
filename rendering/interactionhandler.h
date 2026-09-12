#ifndef INTERACTIONHANDLER_H
#define INTERACTIONHANDLER_H

#include <QObject>
#include <QWidget>
#include "camera.h"

class InteractionHandler : public QObject
{
    Q_OBJECT

    struct DividedPlane {
        QVector3D origin;
        QVector3D size;
        // tile sizes
        QVector3D divisionVec1, divisionVec2;
        DividedPlane(QVector3D _origin, QVector3D _size, QVector3D _divVec1, QVector3D _divVec2)
            : origin(_origin), size(_size), divisionVec1(_divVec1), divisionVec2(_divVec2) { }
    };

public:
    explicit InteractionHandler(Camera *cam, QObject *parent = nullptr);

    bool handleMouseRelease(QMouseEvent* mouseEvent);

signals:

private:
    Camera *camera;
    QList<DividedPlane> planes;
};

#endif // INTERACTIONHANDLER_H
