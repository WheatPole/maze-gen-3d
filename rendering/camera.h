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

    explicit Camera(qreal cameraSpeed, QVector3D cameraPos = QVector3D(2.0, 0, 0), QVector3D cameraTarget = QVector3D(0, 0, 0), bool movable = true, QOpenGLWidget *parent = nullptr);
    QMatrix4x4 getView();
    inline QVector3D position() {
        return _cameraPos;
    }
    inline QMatrix4x4 getProjection() const {
        QMatrix4x4 proj;
        qreal aspect = qreal(screen->width()) / qreal(screen->height() ? screen->height() : 1);
        const qreal zNear = 0.1, zFar = 1500.0, fov = 70.0;

        // Reset projection
        proj.setToIdentity();
        // Set perspective projection
        proj.perspective(fov, aspect, zNear, zFar);
        return proj;
    }
    inline void setPosition(QVector3D newPos) {
        _cameraPos = newPos;
        emit cameraPositionChanged(newPos);
    }

    inline qreal yaw() {
        return _yaw;
    }
    inline qreal pitch() {
        return _pitch;
    }

    inline QVector3D calcCameraFront() {
        QVector3D direction;
        direction.setX(cos(qDegreesToRadians(_yaw)) * cos(qDegreesToRadians(_pitch)));
        direction.setY(sin(qDegreesToRadians(_pitch)));
        direction.setZ(sin(qDegreesToRadians(_yaw)) * cos(qDegreesToRadians(_pitch)));
        return direction.normalized();
    }
    inline QVector3D cameraDirection() const {
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

    constexpr inline QOpenGLWidget* getScreen() {
        return screen;
    }

    bool handleKeyPress(QKeyEvent* keyEvent);
    bool handleKeyRelease(QKeyEvent* keyEvent);
    bool handleMousePress(QMouseEvent* mouseEvent);
    bool handleMouseRelease(QMouseEvent* mouseEvent);
    bool handleWheel(QWheelEvent* wheelEvent);
    bool handleMouseMove(QMouseEvent* mouseEvent);

    bool update();

    constexpr inline double getStaticDistance() { return staticDistance; };
signals:
    void cameraPositionChanged(QVector3D newPos);
private:
    QVector3D _cameraPos;
    QVector3D _cameraTarget;
    QVector3D _cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
    QVector3D _cameraUp = QVector3D(0.0f, 1.0f,  0.0f);
    bool mouseDown = false;
    qreal _yaw, _pitch;
    QPointF lastMousePos;
    qreal _cameraSpeed;
    bool movement[6] = {};
    QOpenGLWidget *screen;

    bool allowMovement;
    // initial distance between camera and target, used mainly for third person perspective
    double staticDistance;
    qreal maxCameraSpeed = 1;
    qreal minCameraSpeed = 0;
    qreal zoomBase = 1.0008;

    void changeCameraDistance(qreal newDistance);
    //QMap<int, bool> keys;
};

#endif // CAMERA_H
