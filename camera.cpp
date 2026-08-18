#include "camera.h"

Camera::Camera(qreal cameraSpeed, QVector3D cameraPos, QVector3D cameraTarget, QOpenGLWidget *parent)
    : QObject(parent) , _cameraSpeed(cameraSpeed), _cameraPos(cameraPos), _cameraTarget(cameraTarget), screen(parent)
{

    maxCameraSpeed = cameraSpeed;
    minCameraSpeed = 0.1;
    screen->installEventFilter(this);

    _yaw = 180;
    _pitch = 0;
    QVector3D direction;
    direction.setX(cos(qDegreesToRadians(_yaw)) * cos(qDegreesToRadians(_pitch)));
    direction.setY(sin(qDegreesToRadians(_pitch)));
    direction.setZ(sin(qDegreesToRadians(_yaw)) * cos(qDegreesToRadians(_pitch)));
    _cameraFront = direction.normalized();
}

QMatrix4x4 Camera::getView() {
    QMatrix4x4 res;
    res.lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
    return res;
}

bool Camera::update() {
    bool changed = false;
    if (movement[Direction::Forward]) {
        //_cameraPos += _cameraSpeed * _cameraFront; changed = true;
        QVector3D delta = _cameraSpeed * _cameraFront; delta.setY(0); _cameraPos += delta.normalized(); changed = true;
    }
    if (movement[Direction::Backward]) {
        //_cameraPos -= _cameraSpeed * _cameraFront; changed = true;
        QVector3D delta = _cameraSpeed * _cameraFront; delta.setY(0); _cameraPos -= delta.normalized(); changed = true;
    }
    if (movement[Direction::Left]) {
        //_cameraPos -= (QVector3D::crossProduct(_cameraFront, _cameraUp)).normalized() * _cameraSpeed; changed = true;
        QVector3D delta = (QVector3D::crossProduct(_cameraFront, _cameraUp)).normalized() * _cameraSpeed; delta.setY(0); _cameraPos -= delta.normalized(); changed = true;
    }
    if (movement[Direction::Right]) {
        //_cameraPos += (QVector3D::crossProduct(_cameraFront, _cameraUp)).normalized() * _cameraSpeed; changed = true;
        QVector3D delta = (QVector3D::crossProduct(_cameraFront, _cameraUp)).normalized() * _cameraSpeed; delta.setY(0); _cameraPos += delta.normalized(); changed = true;
    }
    if (movement[Direction::Up]) {
        _cameraPos += _cameraUp * _cameraSpeed; changed = true;
    }
    if (movement[Direction::Down]) {
        _cameraPos -= _cameraUp * _cameraSpeed; changed = true;
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

bool Camera::eventFilter(QObject* object, QEvent* event) {
    //qDebug() << "Recieved event of type " << event->type() << event;
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
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
    }
    else if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
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
    }
    else if (event->type() == QEvent::MouseButtonPress) {
        // MouseButtomPress will be called before MouseMove
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        lastMousePos = mouseEvent->position();
        mouseDown = true;
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        mouseDown = false;
    }
    else if (event->type() == QEvent::MouseMove) {
        // will be executed only if it's held
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF mousePos = mouseEvent->position();

        QPointF offset = mousePos - lastMousePos;
        lastMousePos = mouseEvent->position();

        float sensitivity = 0.2f;
        offset *= sensitivity;


        _yaw += -offset.x();
        _pitch += offset.y();

        if(_pitch > 89.0f)
            _pitch = 89.0f;
        if(_pitch < -89.0f)
            _pitch = -89.0f;

        QVector3D direction;
        direction.setX(cos(qDegreesToRadians(_yaw)) * cos(qDegreesToRadians(_pitch)));
        direction.setY(sin(qDegreesToRadians(_pitch)));
        direction.setZ(sin(qDegreesToRadians(_yaw)) * cos(qDegreesToRadians(_pitch)));
        _cameraFront = direction.normalized();
    }
    return false;
}
