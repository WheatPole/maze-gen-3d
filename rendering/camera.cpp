#include "camera.h"

Camera::Camera(qreal cameraSpeed, QVector3D cameraPos, QVector3D cameraTarget, bool movable, QOpenGLWidget *parent)
    : QObject(parent) , _cameraSpeed(cameraSpeed), _cameraPos(cameraPos), _cameraTarget(cameraTarget), allowMovement(movable), screen(parent)
{

    maxCameraSpeed = cameraSpeed;
    minCameraSpeed = 0.1;

    staticDistance = (_cameraTarget - _cameraPos).length();

    QVector3D camToTarget = (_cameraTarget - _cameraPos).normalized();
    _yaw = qRadiansToDegrees(atan2(camToTarget.x(), camToTarget.z()));
    _pitch = -qRadiansToDegrees(asin(-camToTarget.y()));

    _cameraFront = calcCameraFront();
}

QMatrix4x4 Camera::getView() {
    QMatrix4x4 res;
    res.lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
    return res;
}

bool Camera::update() {
    bool changed = false;
    if (allowMovement) {
        if (movement[Direction::Forward]) {
            QVector3D delta = _cameraSpeed * _cameraFront; delta.setY(0); _cameraPos += delta.normalized(); changed = true;
        }
        if (movement[Direction::Backward]) {
            QVector3D delta = _cameraSpeed * _cameraFront; delta.setY(0); _cameraPos -= delta.normalized(); changed = true;
        }
        if (movement[Direction::Left]) {
            QVector3D delta = (QVector3D::crossProduct(_cameraFront, _cameraUp)).normalized() * _cameraSpeed; delta.setY(0); _cameraPos -= delta.normalized(); changed = true;    }
        if (movement[Direction::Right]) {
            QVector3D delta = (QVector3D::crossProduct(_cameraFront, _cameraUp)).normalized() * _cameraSpeed; delta.setY(0); _cameraPos += delta.normalized(); changed = true;
        }
        if (movement[Direction::Up]) {
            _cameraPos += _cameraUp * _cameraSpeed; changed = true;
        }
        if (movement[Direction::Down]) {
            _cameraPos -= _cameraUp * _cameraSpeed; changed = true;
        }
    }
    //if (changed) qDebug() << "cam pos" << _cameraPos;
    if (changed) {
        emit cameraPositionChanged(_cameraPos);
    }
    if (mouseDown) {
        return true;
    }
    return changed;
}

bool Camera::handleKeyPress(QKeyEvent* keyEvent) {
    switch (keyEvent->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        movement[Direction::Forward] = true;
        return true;
    case Qt::Key_S:
    case Qt::Key_Down:
        movement[Direction::Backward] = true;
        return true;
    case Qt::Key_A:
    case Qt::Key_Left:
        movement[Direction::Left] = true;
        return true;
    case Qt::Key_D:
    case Qt::Key_Right:
        movement[Direction::Right] = true;
        return true;
    case Qt::Key_Q:
    case Qt::Key_Space:
        movement[Direction::Up] = true;
        return true;
    case Qt::Key_E:
        movement[Direction::Down] = true;
        return true;
    }
    return false;
}

bool Camera::handleKeyRelease(QKeyEvent* keyEvent) {
    switch (keyEvent->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        movement[Direction::Forward] = false;
        return true;
    case Qt::Key_S:
    case Qt::Key_Down:
        movement[Direction::Backward] = false;
        return true;
    case Qt::Key_A:
    case Qt::Key_Left:
        movement[Direction::Left] = false;
        return true;
    case Qt::Key_D:
    case Qt::Key_Right:
        movement[Direction::Right] = false;
        return true;
    case Qt::Key_Q:
    case Qt::Key_Space:
        movement[Direction::Up] = false;
        return true;
    case Qt::Key_E:
        movement[Direction::Down] = false;
        return true;
    }
    return false;
}

bool Camera::handleMousePress(QMouseEvent* mouseEvent) {
    // MouseButtomPress will be called before MouseMove
    if (mouseEvent->button() == Qt::RightButton) {
        lastMousePos = mouseEvent->position();
        mouseDown = true;
    }
    return false;
}

bool Camera::handleMouseRelease(QMouseEvent* mouseEvent) {
    if (mouseEvent->button() == Qt::RightButton)
        mouseDown = false;
    return false;
}

bool Camera::handleWheel(QWheelEvent* wheelEvent) {
    if (wheelEvent->angleDelta().y() != 0) {
        double angle = wheelEvent->angleDelta().y();
        double factor = qPow(zoomBase, angle);
        changeCameraDistance(staticDistance / factor);
        emit cameraPositionChanged(_cameraPos);
    }
    return false;
}

bool Camera::handleMouseMove(QMouseEvent* mouseEvent) {
    if (!mouseDown) return false;
    // will be executed only if it's held
    QPointF mousePos = mouseEvent->position();

    QPointF offset = mousePos - lastMousePos;
    lastMousePos = mouseEvent->position();

    float sensitivity = 0.2f;
    offset *= sensitivity;

    QVector3D camCentre = _cameraPos + calcCameraFront() * staticDistance;

    _yaw -= -offset.x();
    _pitch -= offset.y();

    if(_pitch > 89.0f)
        _pitch = 89.0f;
    if(_pitch < -89.0f)
        _pitch = -89.0f;

    QVector3D eulerVector = calcCameraFront();
    _cameraFront = eulerVector;

    // Third person view addition

    eulerVector *= staticDistance;
    _cameraPos = camCentre - eulerVector;
    return false;
}

void Camera::changeCameraDistance(qreal newDistance) {
    QVector3D eulerVector = calcCameraFront();
    QVector3D camCentre = _cameraPos + eulerVector * staticDistance;
    _cameraPos = camCentre - eulerVector * (staticDistance = newDistance);
}
