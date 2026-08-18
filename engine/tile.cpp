#include "tile.h"
#include <QDebug>

Tile::Tile(QVector3D _position, QVector3D _size, triplet<int> _indices, triplet<int> &mazeDims, double &wallPtg)
    : position(_position), size(_size), indices(_indices), wall(63) {

    std::array<Quad, 6> innerQuads;
    int ind = 0;
    Quad bottomQuad;
    QVector3D up;
    for (const WallFacing facing : WallFacing::allFacings) {
        Quad wallQuad = getWallVertexArray(facing, wallPtg);

        // Retrieving values such that the tileBox has vectors starting
        // from the origin (point with the smallest values), and right, front, and up vectors pointed
        // in the positive direction
        //qDebug() << facing << " " << (facing == WallFacing::XNEG) << " " << (facing == WallFacing::YPOS);
        if (facing == WallFacing::XNEG) up = wallQuad.up;
        else if (facing == WallFacing::YNEG) bottomQuad.origin = wallQuad.origin;
        else if (facing == WallFacing::YPOS) {
            bottomQuad.right = wallQuad.right;
            bottomQuad.up = wallQuad.up;
        }

        Quad reducedQuad(wallQuad);
        // NOTE: this MIGHT not work / works (i think)
        reduceWallVertexArray(facing, wallPtg, reducedQuad);
        QVector3D invNormal = facing.inverse().getNormal();
        QVector3D offset = invNormal * size * wallPtg;

        reducedQuad.origin += offset;

        innerQuads[ind++] = reducedQuad;
    }

    // balancing the edge positions to avoid overlapping
    // TODO: make this look better?
    /*int[] intIndices = { (int)indices.y, (int)indices.z, (int)indices.x};
        int[] bounds = { gen.mazeDims.y, gen.mazeDims.z, gen.mazeDims.x};
        QVector3D[] tboxAxes = { up, bottomQuad.getRight(), bottomQuad.getUp() };
        WallFacing[] facings = { WallFacing.YPOS, WallFacing.ZPOS, WallFacing.XPOS };*/

    QVector3D wallBox = size * wallPtg / 2;
    QVector3D wallX(wallBox.x(), 0, 0);
    QVector3D wallY(0, wallBox.y(), 0);
    QVector3D wallZ(0, 0, wallBox.z());

    if (indices.y > 0) {
        bottomQuad.origin += wallY;
        up -= wallY;
    }
    if (indices.y < mazeDims.y - 1)
        up -= wallY;

    if (indices.z > 0) {
        bottomQuad.origin += wallZ;
        bottomQuad.right -= wallZ;
    }
    if (indices.z < mazeDims.z - 1)
        bottomQuad.right -= wallZ;

    if (indices.x > 0) {
        bottomQuad.origin += wallX;
        bottomQuad.up -= wallX;
    }
    if (indices.x < mazeDims.x - 1)
        bottomQuad.up -= wallX;

    tBox = new TileBox(up, bottomQuad, innerQuads);
}

void Tile::reduceWallVertexArray(const WallFacing &face, double &wallPtg, Quad &wallQuad) {
    QVector3D rightOffset = wallQuad.right.normalized() * size * wallPtg;
    QVector3D upOffset = wallQuad.up.normalized() * size * wallPtg;

    wallQuad.origin += rightOffset + upOffset;

    wallQuad.up -= upOffset * 2;
    wallQuad.right -= rightOffset * 2;
}

Quad Tile::getWallVertexArray(const WallFacing &face, double &wallPtg) {
    // Tile size with walls included
    QVector3D walledSize = size + (size * (wallPtg*2));
    QVector3D normal = face.getNormal();
    //QVector3D invNormal = face.inverse().getNormal();

    // Coordinates start from XNEG and ZNEG, so if normal is facing that way, nullify it
    // Positions the vector so that it's in the (NEG, NEG) corner of the given face
    QVector3D outerWallVector = QVector3D(fmax(0, normal.x()), fmax(0, normal.y()), fmax(0, normal.z()))  * walledSize + position;
    // orthogonal vectors
    QVector3D right = face.right().getNormal(), up = face.up().getNormal();

    right *= walledSize;
    up *= walledSize;

    Quad result = Quad(outerWallVector, right, up);
    return result;
}