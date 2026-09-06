#include "generator.h"
#include "../util/disjointset.h"
#include <algorithm>
#include <random>

Generator::Generator(QVector3D _origin, triplet<int> _mazeDims, QVector3D _bounds, double _wallPercentage)
    : origin(_origin), mazeDims(_mazeDims), bounds(_bounds), wallPtg(_wallPercentage) {

    int xDim = mazeDims.x;
    int yDim = mazeDims.y;
    int zDim = mazeDims.z;

    tileSize = QVector3D(
        bounds.x() / (xDim + wallPtg * (xDim + 1)),
        bounds.y() / (yDim + wallPtg * (yDim + 1)),
        bounds.z() / (zDim + wallPtg * (zDim + 1))
        );

    tileArray = std::vector<Tile*>(xDim * yDim * zDim);
    for (int y = 0; y < yDim; y++) {
        for (int z = 0; z < zDim; z++) {
            for (int x = 0; x < xDim; x++) {
                tileArray[absoluteIndex(y, z, x)] = new Tile(QVector3D(x, y, z) * (tileSize + tileSize * wallPtg),
                                                             tileSize, triplet<int>(x, y, z), mazeDims, wallPtg);
                //tileArray[y][z][x].setWall((byte)63);
            }
        }
    }
}

void Generator::updateTileArray() {
    int xDim = mazeDims.x;
    int yDim = mazeDims.y;
    int zDim = mazeDims.z;

    if (tileArray.size() != xDim * yDim * zDim) {
        tileArray = std::vector<Tile*>(xDim * yDim * zDim);
    }

    for (int y = 0; y < yDim; y++) {
        for (int z = 0; z < zDim; z++) {
            for (int x = 0; x < xDim; x++) {
                Wall wall(63);
                if (tileArray[absoluteIndex(y, z, x)] != nullptr)
                    wall = tileArray[absoluteIndex(y, z, x)]->wall;
                tileArray[absoluteIndex(y, z, x)] = new Tile(QVector3D(x, y, z) * (tileSize + tileSize * wallPtg),
                                                             tileSize, triplet<int>(x, y, z), mazeDims, wallPtg);
                if (tileArray[absoluteIndex(y, z, x)] != nullptr)
                    tileArray[absoluteIndex(y, z, x)]->wall = wall;
            }
        }
    }
}

void Generator::refreshWalls() {
    int xDim = mazeDims.x;
    int yDim = mazeDims.y;
    int zDim = mazeDims.z;

    // Refresh walls
    for (int y = 0; y < yDim; y++) {
        for (int z = 0; z < zDim; z++) {
            for (int x = 0; x < xDim; x++) {
                tileArray[absoluteIndex(y, z, x)]->wall = Wall(63);
            }
        }
    }
}

void Generator::generateWalls() {
    refreshWalls();
    // Kruskal maze generation algorithm
    kruskal();
    applyOpenings();

}

void Generator::applyOpenings() {
    for (IndexFacing opening : openings) {
        auto index = opening.index;
        tileArray[absoluteIndex(index.y, index.z, index.x)]->wall.remove(opening.facing);
    }
}

void Generator::addOpening(triplet<int> index, WallFacing face) {
    if (!valid(index.y, index.z, index.x) || !entryValidity(tileArray[absoluteIndex(index.y, index.z, index.x)], face)) {
        qDebug() << "Invalid opening indices" << index.toString() << face.toString();
        return;
    }
    openings.push_back({index, face});
    tileArray[absoluteIndex(index.y, index.z, index.x)]->wall.remove(face);
}

void Generator::eraseOpening(int ind) {
    auto index = openings[ind].index;
    tileArray[absoluteIndex(index.y, index.z, index.x)]->wall.add(openings[ind].facing);
    openings.erase(openings.begin()+ind);
}

bool Generator::entryValidity(Tile* entryTile, WallFacing entryFace) {
    QVector3D entranceFaceOffset = entryFace.getNormal() / 100;

    QVector3D startVec(entryTile->position);
    startVec = startVec + entranceFaceOffset;
    QVector3D offVec = startVec + tileSize + tileSize * (2 * wallPtg);

    auto isInside = [](QVector3D bounds1, QVector3D vec, QVector3D pivot = QVector3D(0, 0, 0)) {
        bool valX = (vec.x() >= pivot.x() && vec.x() <= (pivot.x() + bounds1.x()));
        bool valY = (vec.y() >= pivot.y() && vec.y() <= (pivot.y() + bounds1.y()));
        bool valZ = (vec.z() >= pivot.z() && vec.z() <= (pivot.z() + bounds1.z()));

        return valX && valY && valZ;
    };

    bool outS1 = !isInside(bounds, startVec), outS2 = !isInside(bounds, offVec);
    // Both vectors need to be outside, so that the facings are actually facing outside
    return outS1 || outS2;
}

void Generator::kruskal() {

    // needs hashing
    struct WallData {
    public:
        triplet<int> indices;
        WallFacing facing;
        WallData(triplet<int> _indices, WallFacing _facing) : indices(_indices), facing(_facing) {

        }
    };

    std::vector<WallData> wallData;
    DisjointSet<triplet<int>> cellSet;

    WallFacing positive[] = { WallFacing::XPOS, WallFacing::YPOS, WallFacing::ZPOS };

    for (int y = 0; y < mazeDims.y; y++) {
        for (int z = 0; z < mazeDims.z; z++) {
            for (int x = 0; x < mazeDims.x; x++) {
                long id = x + (long)z * mazeDims.x + (long)y * mazeDims.x * mazeDims.z;
                triplet<int> ind = tileArray[absoluteIndex(y, z, x)]->indices;

                cellSet.add(ind);

                for (WallFacing dir : positive) {
                    if (!(dir == WallFacing::XNEG && x == 0) &&
                        !(dir == WallFacing::XPOS && x == mazeDims.x-1) &&
                        !(dir == WallFacing::YNEG && y == 0) &&
                        !(dir == WallFacing::YPOS && y == mazeDims.y-1) &&
                        !(dir == WallFacing::ZNEG && z == 0) &&
                        !(dir == WallFacing::ZPOS && z == mazeDims.z-1)
                        ) {
                        wallData.push_back(WallData(ind, dir));
                    }
                }
            }
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(wallData.begin(), wallData.end(), g);

    for (WallData data : wallData) {
        triplet<int> startIndices = data.indices;
        WallFacing facing = data.facing;
        Tile* current =  tileArray[absoluteIndex(startIndices.y, startIndices.z, startIndices.x)];

        QVector3D vec = facing.getNormal();
        triplet<int> nextInd = startIndices + vec;
        Tile* next = tileArray[absoluteIndex(nextInd.y,nextInd.z,nextInd.x)];
        bool hasInd =  (cellSet.find(startIndices) == cellSet.find(next->indices));

        // If not under the same parent
        if (!hasInd) {
            current->wall.remove(facing);
            next->wall.remove(facing.inverse());

            cellSet.join(startIndices, next->indices);
        }
        else {
            // probabilistic loops?
        }
    }
}


void Generator::setDims(triplet<int> newDims) {
    for (IndexFacing& opening : openings) {
        triplet<int> &ind = opening.index;
        if (ind.x == mazeDims.x-1 && mazeDims.x < newDims.x) {
            ind.x = newDims.x-1;
        }
        if (ind.y == mazeDims.y-1 && mazeDims.y < newDims.y) {
            ind.y = newDims.y-1;
        }
        if (ind.z == mazeDims.z-1 && mazeDims.z < newDims.z) {
            ind.z = newDims.z-1;
        }
    }

    mazeDims = newDims;

    applyOpenings();
}
