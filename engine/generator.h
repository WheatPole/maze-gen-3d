#ifndef GENERATOR_H
#define GENERATOR_H

#include "tile.h"
#include <QObject>

class Generator : public QObject {
    Q_OBJECT

public:
    struct IndexFacing {
        triplet<int> index;
        WallFacing facing;
        IndexFacing(triplet<int> _index, WallFacing _facing) : index(_index), facing(_facing) {}
    };
    // Origin - the bottom corner from which the generator will build
    QVector3D origin;
    triplet<int> mazeDims;
    QVector3D bounds;
    // Plain tile size, without walls
    QVector3D tileSize;
    // Outer-wall openings
    std::vector<IndexFacing> *openings;
    // Order of axes - Y, Z, X, all tiles start with walls on all sides
    // Z -> up/down, X -> left/right
    std::vector<Tile*> tileArray;

    bool*** buffer;
    double wallPtg;

    Generator(QVector3D _origin, triplet<int> _mazeDims, QVector3D _bounds, double wallPercentage);

    // Updates tiles (and tileboxes within them) to fit the bounds and dims
    // Reset indicates whether the tile array should be reset or not
    // It is highly recommended to RESET the array when working
    // with new maze dimensions, due to the structure of the array (1 dimensional array)
    void updateTiles(bool resetWalls, bool resetDims);

    inline double wallWidth(WallFacing dir) const {
        if (dir == WallFacing::XNEG || dir == WallFacing::XPOS) {
            return tileSize.x() * wallPtg;
        }
        else if (dir == WallFacing::ZNEG || dir == WallFacing::ZPOS) {
            return tileSize.z() * wallPtg;
        }
        else if (dir == WallFacing::YNEG || dir == WallFacing::YPOS) {
            return tileSize.y() * wallPtg;
        }

        return -1;
    }

    constexpr inline int absoluteIndex(int y, int z, int x) const {
        return y * mazeDims.x * mazeDims.z + z * mazeDims.x + x;
    }
    inline int absoluteIndex(triplet<int> index) const {
        return index.y * mazeDims.x * mazeDims.z + index.z * mazeDims.x + index.x;
    }

    void generateWalls();

    void addOpening(triplet<int> index, WallFacing face);
    void eraseOpening(int index);
    void applyOpenings();
    bool entryValidity(QVector3D &position, WallFacing &entryFace);

    void kruskal();

    inline int random(int min, int max) {
        //System.out.println(min + " " + max);
        if (min >= max) return min;
        return rand() % (max - min + 1) + min;
    }

    inline bool valid(int y, int z, int x) {
        return (x >= 0 && x < mazeDims.x &&
                y >= 0 && y < mazeDims.y &&
                z >= 0 && z < mazeDims.z);
    }
    inline std::string toString() const {
        int n = mazeDims.x;
        std::vector<std::string> mazeStr(n*3);
        for (int i = 0; i < 3*n; i++) {
            mazeStr[i] = "";
        }

        for (int i = 0; i < mazeDims.z; i++) {
            for (int j = 0; j < mazeDims.x; j++) {
                std::array<std::string, 3> currString = tileArray[absoluteIndex(0, i, j)]->toString();
                //System.out.printf("Got this for %d %d: %s %s %s\n", i, j, currString[0], currString[1], currString[2]);
                mazeStr[3*i] = mazeStr[3*i] + currString[0];
                mazeStr[3*i + 1] = mazeStr[3*i + 1] + currString[1];
                mazeStr[3*i + 2] = mazeStr[3*i + 2] + currString[2];
            }
        }
        std::string res = "";
        for (std::string str : mazeStr) {
            res += str + '\n';
        }
        return res;
    }
    void setDims(triplet<int> newDims);
    void setBounds(QVector3D newBounds);
    void setWallPtg(double val);

signals:
    void openingInvolutarelyChanged(int ind, IndexFacing opening);
    void boundsChanged(QVector3D newBounds);
};
#endif // GENERATOR_H
