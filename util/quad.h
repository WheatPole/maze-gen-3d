#ifndef QUAD_H
#define QUAD_H

#include <QVector3D>

class Quad {
public:
    QVector3D origin;
    // Follows a counter-clockwise canonical ordering.
    // Right should be a vector that's visibly "right" from the origin,
    // while up is the vector that's rotated counter-clockwise from right
    QVector3D right, up;
    Quad(QVector3D _origin = QVector3D(0, 0, 0), QVector3D _right = QVector3D(0, 0, 0), QVector3D _up = QVector3D(0, 0, 0)) {
        origin = _origin;
        right = _right;
        up = _up;
    }
    Quad(Quad &quad) {
        origin = quad.origin;
        right = quad.right;
        up = quad.up;
    }
    Quad addToOrigin(QVector3D vec) {
        // better to copy all attributes instead of assigning them?
        return Quad(origin + vec, right, up);
    }
};


#endif // QUAD_H
