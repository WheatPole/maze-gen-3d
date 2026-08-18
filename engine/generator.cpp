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

// Index arrays must follow the following format: [x,y,z]
void Generator::generateWalls(triplet<int> startIndices, WallFacing entryFace, triplet<int> finishIndices, WallFacing exitFace) {
    int xDim = mazeDims.x;
    int yDim = mazeDims.y;
    int zDim = mazeDims.z;
    Tile* startTile = tileArray[absoluteIndex(startIndices.y, startIndices.z, startIndices.x)];
    Tile* finishTile = tileArray[absoluteIndex(finishIndices.y, finishIndices.z, finishIndices.x)];

    if (!entryValidity(startTile, entryFace, finishTile, exitFace)) {
        //System.out.println("Invalid initial facing values during generation");
        return;
    }

    // Refresh walls
    for (int y = 0; y < yDim; y++) {
        for (int z = 0; z < zDim; z++) {
            for (int x = 0; x < xDim; x++) {
                tileArray[absoluteIndex(y, z, x)]->wall = Wall(63);
            }
        }
    }

    // Begins the generation
    kruskal(startIndices, entryFace, finishIndices);
    //randomIndexFilling(startIndices, entryFace, finishIndices);
    //propagate(startIndices, entryFace, finishIndices);
    finishTile->wall.remove(exitFace);
}

bool Generator::entryValidity(Tile* entryTile, WallFacing entryFace, Tile* exitTile, WallFacing exitFace) {
    QVector3D entranceFaceOffset = entryFace.getNormal() / 100;
    QVector3D exitFaceOffset = exitFace.getNormal() / 100;

    QVector3D startVec(entryTile->position), finishVec(exitTile->position);
    startVec = startVec + entranceFaceOffset;
    finishVec = finishVec + exitFaceOffset;
    QVector3D startOffVec = startVec + tileSize + tileSize * (2 * wallPtg),
        finishOffVec = finishVec + tileSize + tileSize * (2 * wallPtg);

    auto isInside = [](QVector3D bounds1, QVector3D vec, QVector3D pivot = QVector3D(0, 0, 0)) {
        bool valX = (vec.x() >= pivot.x() && vec.x() <= (pivot.x() + bounds1.x()));
        bool valY = (vec.y() >= pivot.y() && vec.y() <= (pivot.y() + bounds1.y()));
        bool valZ = (vec.z() >= pivot.z() && vec.z() <= (pivot.z() + bounds1.z()));

        return valX && valY && valZ;
    };

    bool outS1 = !isInside(bounds, startVec), outS2 = !isInside(bounds, startOffVec);
    bool outF1 = !isInside(bounds, finishVec), outF2 = !isInside(bounds, finishOffVec);
    // Both vectors need to be outside, so that the facings are actually facing outside
    return (outS1 || outS2) && (outF1 || outF2);
}

void Generator::kruskal(triplet<int> currentInd, WallFacing entryFace, triplet<int> goalInd) {
    int y1 = currentInd.y, z1 = currentInd.z, x1 = currentInd.x;
    //buffer = new boolean[mazeDims.y][mazeDims.z][mazeDims.x];
    tileArray[absoluteIndex(y1,z1,x1)]->wall.remove(entryFace);

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