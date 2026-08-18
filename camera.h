#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMap>

class Camera : public QObject
{
    Q_OBJECT
public:
    enum Direction {
        Forward = 0,
        Backward = 1,
        Left = 2,
        Right = 3,
        Up = 4,
        Down = 5
    };

    explicit Camera(qreal cameraSpeed, QVector3D cameraPos = QVector3D(2.0, 0, 0), QVector3D cameraTarget = QVector3D(0, 0, 0), QOpenGLWidget *parent = nullptr);
    QMatrix4x4 getView();
    inline QVector3D position() {
        return _cameraPos;
    }
    inline qreal yaw() {
        return _yaw;
    }
    inline qreal pitch() {
        return _pitch;
    }

    inline QVector3D cameraDirection() {
        return (_cameraPos - _cameraTarget).normalized();
    }
    inline QVector3D cameraRight();

    void moveBy(qreal dx, qreal dy, qreal dz) {
        if (dx != 0)
            _cameraPos.setX(_cameraPos.x() + dx);
        if (dy != 0)
            _cameraPos.setY(_cameraPos.y() + dy);
        if (dz != 0)
            _cameraPos.setZ(_cameraPos.z() + dz);
    }
    void setX(qreal val) {
        _cameraPos.setX(val);
    }
    void setY(qreal val) {
        _cameraPos.setY(val);
    }
    void setZ(qreal val) {
        _cameraPos.setZ(val);
    }

    bool eventFilter(QObject* object, QEvent* event) override;
    bool update();
    QVector3D _cameraPos;
signals:
    void cameraPositionChanged(QVector3D newPos);
private:
    QVector3D _cameraTarget;
    QVector3D _cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
    QVector3D _cameraUp = QVector3D(0.0f, 1.0f,  0.0f);
    bool mouseDown = false;
    qreal _yaw, _pitch;
    QPointF lastMousePos;
    qreal _cameraSpeed;
    bool movement[6] = {};
    QOpenGLWidget *screen;

    // slower movement when going towards the ground
    bool realisticCamera = true;
    qreal maxCameraSpeed = 1;
    qreal minCameraSpeed = 0;
    //QMap<int, bool> keys;
};

#endif // CAMERA_H
