#ifndef GENERATOR_H
#define GENERATOR_H

#include "tile.h"
#include <QObject>

class Generator : public QObject {
    Q_OBJECT

    struct IndexFacing {
        triplet<int> index;
        WallFacing facing;
        IndexFacing(triplet<int> _index, WallFacing _facing) : index(_index), facing(_facing) {}
    };

public:
    // Origin - the bottom corner from which the generator will build
    QVector3D origin;
    triplet<int> mazeDims;
    QVector3D bounds;
    // Plain tile size, without walls
    QVector3D tileSize;
    // Outer-wall openings
    std::vector<IndexFacing> openings;
    // Order of axes - Y, Z, X, all tiles start with walls on all sides
    // Z -> up/down, X -> left/right
    std::vector<Tile*> tileArray;

    bool*** buffer;
    double wallPtg;
    // todo? add an additional parameter for floor (box) height

    Generator(QVector3D _origin, triplet<int> _mazeDims, QVector3D _bounds, double wallPercentage);
    void updateTileArray();

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

    inline int absoluteIndex(int y, int z, int x) const {
        return y * mazeDims.x * mazeDims.z + z * mazeDims.x + x;
    }

    void refreshWalls();
    void generateWalls();

    void addOpening(triplet<int> index, WallFacing face);
    void eraseOpening(int index);
    void applyOpenings();
    bool entryValidity(Tile* entryTile, WallFacing entryFace);

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
    inline void setBounds(QVector3D newBounds) {
        bounds = newBounds;
    }
    inline void setWallPtg(double val) {
        wallPtg = val;
    }

signals:
    // implement
    void openingChanged(int ind);
};
#endif // GENERATOR_H
