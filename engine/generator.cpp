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

void Generator::updateTiles(bool resetWalls, bool resetDims) {
    int xDim = mazeDims.x;
    int yDim = mazeDims.y;
    int zDim = mazeDims.z;

    if (resetDims) {
        if (tileArray.size() != xDim * yDim * zDim) {
            //tileArray.clear();
            tileArray.resize(xDim * yDim * zDim);
        }
    }

    // Refresh walls
    for (int y = 0; y < yDim; y++) {
        for (int z = 0; z < zDim; z++) {
            for (int x = 0; x < xDim; x++) {
                int ind = absoluteIndex(y, z, x);
                Wall tempWall(63);
                if (tileArray[ind] != nullptr && !resetWalls) {
                    tempWall = tileArray[ind]->wall;
                }
                if (resetDims)
                    tileArray[ind] = new Tile(QVector3D(x, y, z) * (tileSize + tileSize * wallPtg),
                                                             tileSize, triplet<int>(x, y, z), mazeDims, wallPtg);
                tileArray[ind]->wall = tempWall;

            }
        }
    }
}

void Generator::generateWalls() {
    updateTiles(1, 0);
    // Kruskal maze generation algorithm
    kruskal();
    applyOpenings();

}

void Generator::applyOpenings() {
    for (IndexFacing opening : openings) {
        auto index = opening.index;
        qDebug() << "Removing wall at " << index.toString() << absoluteIndex(index.y, index.z, index.x);
        tileArray[absoluteIndex(index.y, index.z, index.x)]->wall.remove(opening.facing);
    }
}

void Generator::addOpening(triplet<int> index, WallFacing face) {
    if (!valid(index.y, index.z, index.x) || !entryValidity(tileArray[absoluteIndex(index)]->position, face)) {
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

bool Generator::entryValidity(QVector3D &position, WallFacing &entryFace) {
    QVector3D entranceFaceOffset = entryFace.getNormal() / 100;

    QVector3D startVec(position);
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
    int iter = 0;
    triplet<int> delta = newDims - mazeDims;
    qDebug() << bounds << tileSize << delta.toString() << newDims.toString();
    bounds += (tileSize * (1 + wallPtg)) * QVector3D(delta.x, delta.y, delta.z);
    qDebug() << bounds;
    auto oldDims = mazeDims;
    mazeDims = newDims;
    updateTiles(1, 1);
    for (IndexFacing& opening : openings) {
        triplet<int> oldInd = opening.index;
        triplet<int> &ind = opening.index;
        bool changed = false;
        if (ind.x == oldDims.x-1 && oldDims.x != newDims.x) {
            ind.x = newDims.x-1;
            changed = true;
        }
        if (ind.y == oldDims.y-1 && oldDims.y != newDims.y) {
            ind.y = newDims.y-1;
            changed = true;
        }
        if (ind.z == oldDims.z-1 && oldDims.z != newDims.z) {
            ind.z = newDims.z-1;
            changed = true;
        }
        //qDebug() << ind.toString() << entryValidity(tileArray[absoluteIndex(ind)]->position, opening.facing);
        if (changed && !entryValidity(tileArray[absoluteIndex(ind)]->position, opening.facing)) {
            ind = oldInd;
            changed = false;
        }

        if (changed) emit openingInvolutarelyChanged(iter, opening);
        iter++;
        //qDebug() << ind.toString();
    }

    emit boundsInvolutarelyChanged(bounds);
    applyOpenings();
}

void Generator::setWallPtg(double val) {
    int xDim = mazeDims.x;
    int yDim = mazeDims.y;
    int zDim = mazeDims.z;

    wallPtg = val;
    tileSize = QVector3D(
        bounds.x() / (xDim + wallPtg * (xDim + 1)),
        bounds.y() / (yDim + wallPtg * (yDim + 1)),
        bounds.z() / (zDim + wallPtg * (zDim + 1))
        );
    updateTiles(0, 1);
}

void Generator::setBounds(QVector3D newBounds) {
    bounds = newBounds;

    int xDim = mazeDims.x;
    int yDim = mazeDims.y;
    int zDim = mazeDims.z;

    tileSize = QVector3D(
        bounds.x() / (xDim + wallPtg * (xDim + 1)),
        bounds.y() / (yDim + wallPtg * (yDim + 1)),
        bounds.z() / (zDim + wallPtg * (zDim + 1))
        );

    updateTiles(0, 1);
}
