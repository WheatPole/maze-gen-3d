#ifndef TILEBOX_H
#define TILEBOX_H

#include "../util/quad.h"
#include "wall.h"
#include <array>

// Data class for containing data about tile vertices
// It handles coordinates and vectors, while VertexBox handles vertices and indices
// used for rendering and writing files
class TileBox {
private:
public:
    // Should be defined in general order of XNEG -> ZPOS
    std::array<Quad, 6> innerQuads;
    // Rects should be defined from the smallest mod and then in canonical order
    // y vector
    QVector3D up;
    // xz at bottom, right - XPOS, up - ZPOS
    // NOTE: THIS HAS A NORMAL TOWARDS YPOS, NOT YNEG
    // THIS IS JUST A WAY TO STORE right, origin AND front VECTORS
    Quad bottomQuad;

    TileBox(QVector3D _up, Quad _bottomQuad, std::array<Quad, 6> _innerQuads);

    Quad getQuad(WallFacing facing);
    Quad getInnerQuad(WallFacing facing);
};

#endif // TILEBOX_H
