#include "interactionhandler.h"

InteractionHandler::InteractionHandler(Camera *cam, QObject *parent)
    : camera(cam), QObject{parent}
{

}

bool InteractionHandler::handleMouseRelease(QMouseEvent* mouseEvent) {
    QPoint viewportPos = mouseEvent->pos();
    QPoint viewportBounds = QPoint(camera->getScreen()->width(), camera->getScreen()->height());

    QVector3D normalized((viewportPos.x() * 2.0)/viewportBounds.x() - 1.0,
                         1.0 - (viewportPos.x() * 2.0)/viewportBounds.y(),
            1.0);

    // raycasting
    QVector4D rayClip(normalized.x(), normalized.y(), -1.0, 1.0);

    QMatrix4x4 proj = camera->getScreen()->


    return false;
}