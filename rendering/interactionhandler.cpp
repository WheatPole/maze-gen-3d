#include "interactionhandler.h"

InteractionHandler::InteractionHandler(Camera *cam, QObject *parent)
    : camera(cam), QObject{parent}
{

}

bool isInsidePlane(QVector3D point, InteractionHandler::Plane plane, float epsilon)
{
    QVector3D min = plane.origin - QVector3D(epsilon, epsilon, epsilon);
    QVector3D max = plane.origin + plane.size + QVector3D(epsilon, epsilon, epsilon);

    return point.x() >= min.x() && point.x() <= max.x()
           && point.y() >= min.y() && point.y() <= max.y()
           && point.z() >= min.z() && point.z() <= max.z();
}

bool InteractionHandler::handleMouseRelease(QMouseEvent* mouseEvent) {
    if (mouseEvent->button() == Qt::RightButton) return false;
    QPoint viewportPos = mouseEvent->pos();
    QPoint viewportBounds = QPoint(camera->getScreen()->width(), camera->getScreen()->height());

    // raycasting
    QVector4D rayClip((viewportPos.x() * 2.0)/viewportBounds.x() - 1.0, 1.0 - (viewportPos.y() * 2.0)/viewportBounds.y(), -1.0, 1.0);

    QMatrix4x4 proj = camera->getProjection(), view = camera->getView();
    //qDebug() << rayClip;

    QVector4D cameraProjectedRay = proj.inverted() * rayClip;
    cameraProjectedRay.setZ(-1.0);
    cameraProjectedRay.setW(0.0);

    QVector4D ray4D = view.inverted() * cameraProjectedRay;
    QVector3D ray3D(ray4D.x(), ray4D.y(), ray4D.z());
    ray3D.normalize();

    //qDebug() << "Ray: " << ray3D;
    QVector3D rayOrigin = camera->position();
    Plane closestPlane;
    float minDist = 99999999;
    QVector3D rayHit;
    for (Plane plane : planes) {
        // check if ray3D intersects the plane, then find the closest intersection
        // line: [x y z] = vec * k + origin
        // plane: (r1 - r) dot normal = 0
        // ([x y z] - pOrigin) dot normal = 0
        // solve for k -> ((vec * k + origin) - pOrigin) dot normal = 0
        QVector3D dist = rayOrigin - plane.origin;
        // (k * vec.x + dist.x) * normal.x + ... = 0
        float k = -(QVector3D::dotProduct(dist, plane.normal)) / (QVector3D::dotProduct(ray3D, plane.normal));
        QVector3D intersectionPoint = ray3D * k + rayOrigin;
        //qDebug() << "Got " << k << " of " << intersectionPoint << " for " << plane.origin << plane.size << plane.normal;
        if (k < 0) continue;
        if (isInsidePlane(intersectionPoint, plane, 1e-5)) {
            //qDebug() << "Ray inside " << plane.origin << plane.size << "(ray " << intersectionPoint << ")!";
            float dist = (intersectionPoint - rayOrigin).length();
            if (dist < minDist) {
                closestPlane = plane;
                minDist = dist;
                rayHit = intersectionPoint;
            }
        }
    }

    // not the best idea in practice ( < 100000)
    if (minDist < 100000) {
        qDebug() << "Interacted with plane " << closestPlane.origin << closestPlane.size;
        emit intersectedPlane(closestPlane, rayHit);
    }
    else {
        emit noIntersections();
    }

    return false;
}
