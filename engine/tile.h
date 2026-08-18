#ifndef TILE_H
#define TILE_H

#include "wall.h"
#include "tilebox.h"
#include "../util/triplet.h"
#include <array>

class Tile {
public:
    Wall wall;
    // Tile position
    QVector3D position;
    // Plain tile size, without walls
    QVector3D size;
    TileBox *tBox;
    triplet<int> indices;

    Tile() : wall(Wall(63)), indices(triplet<int>(0,0,0)) {

    }
    Tile(QVector3D _position, QVector3D _size, triplet<int> _indices, triplet<int> &mazeDimensions, double &wallPtg);

    // Theoretically should just give the vertex array of the inner tile wall (no wall padding)
    void reduceWallVertexArray(const WallFacing &face, double &wallPtg, Quad &wallQuad);

    // Returns the vertex array of the wall for a given face and wall width percentage
    // First 4 elements represent the outer wall vertices, next 4 represent the inner wall vertices
    // Last 8 elements represent outer and inner edge pillar vertices, respectively
    // spliceDirection - direction based on which edge pillars will be cut from the wall
    // Resulting pillars are parallel to spliceDirection direction
    Quad getWallVertexArray(const WallFacing &face, double &wallPtg);

    inline std::array<std::string, 3> toString() const {
        std::array<std::string, 3> result = {
            "###", "# #", "###"
        };
        if (!wall.exists(WallFacing::ZNEG)) {
            result[0][1] = ' ';
        }
        if (!wall.exists(WallFacing::ZPOS)) {
            result[2][1] = ' ';
        }
        if (!wall.exists(WallFacing::XNEG)) {
            result[1][0] = ' ';
        }

        if (!wall.exists(WallFacing::XPOS)) {
            result[1][2] = ' ';
        }
        return result;
    }
};

#endif // TILE_H
