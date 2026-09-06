#include "mesh.h"
#include <QDebug>

Mesh::Mesh(Generator *_generator) : generator(_generator) {
    // Premise is to construct vertices tile by tile, and for each tile,
    // we generate vertices for the following:
    // - Outer walls (only if next to the edge, with wall padding)
    // - Inner walls (without wall padding)
    // because this is essentially minimal data needed to properly construct the maze
    auto mazeDims = generator->mazeDims;
    vBoxList = std::vector<VertexBox*>(mazeDims.y * mazeDims.z * mazeDims.x);

    m_size = 0;
    // 4 * 2 for opening (change to 4 * n when added functionality)
    maxSize = (mazeDims.x+1) * (mazeDims.z+1) * 2
              + (mazeDims.x+1) * (mazeDims.y+1) * 2
              + (mazeDims.z+1) * (mazeDims.y+1) * 2
              //- 2 * (mazeDims.x + mazeDims.y + mazeDims.z)
              + 4 * 2;
    updateVbox();

    refresh();
}

void Mesh::updateVbox() {
    auto mazeDims = generator->mazeDims;
    if (vBoxList.size() != mazeDims.y * mazeDims.z * mazeDims.x) {
        vBoxList.resize(mazeDims.y * mazeDims.z * mazeDims.x);
    }
    // Traverse order: Y, Z, X
    for (int y = 0; y < mazeDims.y; y++) {
        for (int z = 0; z < mazeDims.z; z++) {
            for (int x = 0; x < mazeDims.x; x++) {
                long index = generator->absoluteIndex(y,z,x);
                Tile* tile = generator->tileArray[index];
                TileBox* tBox = tile->tBox;
                vBoxList[index] = new VertexBox(*tBox);
            }
        }
    }
}

// Orients the vertices such that the normal becomes <normal>
std::array<QVector3D, 4> orient(std::array<QVector3D, 4> vertices, QVector3D normal) {
    std::array<QVector3D, 4> res = {};
    int minIndex = 0, maxIndex = 0;
    double minMod = 9999999, maxMod = 0;
    for (int i = 0; i < 4; i++) {
        const QVector3D vec = vertices[i];
        if (vec.length() < minMod) {
            minIndex = i;
            minMod = vec.length();
        }
        if (vec.length() > maxMod) {
            maxIndex = i;
            maxMod = vec.length();
        }
    }
    QVector3D minVec = res[0] = vertices[minIndex];
    QVector3D maxVec = res[2] = vertices[maxIndex];

    for (int i = 0; i < 4; i++) {
        const QVector3D vec = vertices[i];
        if (i != minIndex && i != maxIndex) {
            QVector3D vec1 = vec - minVec;
            QVector3D vec2 = maxVec - vec;
            QVector3D cross = QVector3D::crossProduct(vec1, vec2).normalized();

            // if oriented with normal
            if ((cross - normal).length() < 1) {
                res[1] = vec;
            }
            else {
                res[3] = vec;
            }
        }
    }
    return res;
}

void Mesh::fillInnerVertices() {
    // Wall facing vertex append order: XNEG, XPOS, ZNEG, ZPOS, YNEG, YPOS
    auto mazeDims = generator->mazeDims;
    // Inner faces
    for (int y = 0; y < mazeDims.y; y++) {
        for (int z = 0; z < mazeDims.z; z++) {
            for (int x = 0; x < mazeDims.x; x++) {
                int index = generator->absoluteIndex(y,z,x);
                Tile *tile = generator->tileArray[index];
                TileBox *tBox = tile->tBox;
                VertexBox *vBox = vBoxList[index];

                for (const WallFacing facing : WallFacing::allFacings) {
                    QVector3D normal = facing.getNormal();
                    int x1 = x + (int) normal.x();
                    int y1 = y + (int) normal.y();
                    int z1 = z + (int) normal.z();
                    int nextIndex = generator->absoluteIndex(y1,z1,x1);

                    // in-bound processing
                    std::array<QVector3D, 4> innerVertices = vBox->getIWVertices(facing);
                    //std::array<int, 4> processedIndices = transcribeVertices(innerVertices);

                    // Only add gateway faces when looking at positive direction
                    // (to avoid duplicates, as from the concept, walls are overlapped)
                    if (tile->wall.exists(facing)) {
                        //qDebug() << facing << " " << facing.inverse();
                        faces.push_back(new Face(
                            transcribeVertices(orient(innerVertices, facing.inverse().getNormal())),
                            normalIndex(facing.inverse())
                            ));
                    } else {
                        // If facing outside -> handled by out-bound processor
                        if (facing.isPositive() && generator->valid(y1, z1, x1)) {
                            VertexBox* adjVBox = vBoxList[nextIndex];
                            std::array<int, 4> adjIndices = adjVBox->getIWIndices(facing.inverse());
                            std::array<QVector3D, 4> adjVertices = adjVBox->getIWVertices(facing.inverse());
                            std::array<int, 4> processedAdjIndices = transcribeVertices(adjVertices);

                            // Vector from which the face construction starts
                            QVector3D startVector = innerVertices[0];
                            int oppositeIndex = lookupVertex(facing, adjIndices, startVector, *adjVBox);
                            if (oppositeIndex == -1) {
                                qFatal() << "ERROR, couldn't find the opposite index " << startVector << " " << adjIndices;
                                continue;
                            }

                            // It's enough to only look up one index, as the indices are
                            // laid out such that opposite sides follow opposite traversals
                            // (clockwise and anti-clockwise traversal pairs)

                            WallFacing gatewayFacing = facing.up();
                            for (int i = 0; i < 4; i++) {
                                int i1 = i, j1 = (oppositeIndex - i + 4) % 4;
                                int i2 = (i1 + 1) % 4, j2 = (j1 - 1 + 4) % 4;

                                auto orientedVertices = orient({innerVertices[i1], innerVertices[i2], adjVertices[j2], adjVertices[j1]}, gatewayFacing.getNormal());
                                faces.push_back(new Face(transcribeVertices(orientedVertices),
                                                         normalIndex(gatewayFacing)
                                                   ));

                                gatewayFacing = gatewayFacing.rotate(facing);
                            }
                        }
                    }
                }
            }
        }
    }

}

void Mesh::fillOuterVertices() {
    // Wall facing vertex append order: XNEG, XPOS, ZNEG, ZPOS, YNEG, YPOS
    auto mazeDims = generator->mazeDims;
    for (int y = 0; y < mazeDims.y; y++) {
        for (int z = 0; z < mazeDims.z; z++) {
            for (int x = 0; x < mazeDims.x; x++) {
                int index = generator->absoluteIndex(y,z,x);
                Tile* tile = generator->tileArray[index];
                TileBox* tBox = tile->tBox;
                VertexBox *vBox = vBoxList[index];

                for (WallFacing facing : WallFacing::allFacings) {
                    QVector3D normal = facing.getNormal();
                    int x1 = x + (int) normal.x();
                    int y1 = y + (int) normal.y();
                    int z1 = z + (int) normal.z();

                    // out-bound processing
                    if (!generator->valid(y1, z1, x1)) {
                        std::array<QVector3D, 4> outerVertices = vBox->getOWVertices(facing);

                        // reduce edges so the resulting face doesn't overlap other faces
                        // reduces on edges that have 2 non-perpendicular sides
                        /*WallFacing rot = facing.up();
                            for (int ind = 0; ind < outerVertices.length; ind++) {
                                Vector3D invRot = rot.inverse().getNormal();
                                if (generator.valid(
                                        y + (int)invRot.y,
                                        z + (int)invRot.z,
                                        x + (int)invRot.x)
                                ) {
                                    outerVertices[ind] = outerVertices[ind].add(rot.getNormal().mul(generator.wallWidth(rot)/2));
                                    outerVertices[(ind + 1) % 4] = outerVertices[(ind + 1) % 4].add(rot.getNormal().mul(generator.wallWidth(rot)/2));
                                }

                                rot = rot.rotate(facing);
                            }*/

                        std::array<int, 4> outerIndices = transcribeVertices(outerVertices);
                        if (tile->wall.exists(facing)) {
                            faces.push_back(new Face(
                                outerIndices,
                                normalIndex(facing)
                                ));
                        }
                        // Only happens on the entry/exit of the maze
                        else {
                            Quad innerWall = tBox->getInnerQuad(facing);
                            // apply wall offset and then construct 4 quads to connect it all

                            QVector3D offset = facing.getNormal() * generator->wallWidth(facing);

                            innerWall = innerWall.addToOrigin(offset);

                            std::array<QVector3D, 4> extraVertices = {
                                innerWall.origin,
                                innerWall.origin + innerWall.right,
                                innerWall.origin + innerWall.right + innerWall.up,
                                innerWall.origin + innerWall.up
                            };

                            std::array<int, 4> extraIndices = transcribeVertices(extraVertices);
                            // outer wall construction
                            for (int i = 0; i < 4; i++) {
                                int i1 = i, i2 = (i + 1)%4;
                                faces.push_back(new Face({
                                                       outerIndices[i1], outerIndices[i2],
                                                       extraIndices[i2], extraIndices[i1]
                                                   },
                                                         normalIndex(facing)
                                                   ));
                            }

                            std::array<QVector3D, 4> innerVertices = vBox->getIWVertices(facing);
                            //std::array<int, 4> innerIndices = transcribeVertices(innerVertices);
                            // gateway construction

                            WallFacing gatewayFacing = facing.up();
                            for (int i = 0; i < 4; i++) {
                                int i1 = i, i2 = (i + 1)%4;
                                auto orientedVertices = orient({extraVertices[i1], extraVertices[i2], innerVertices[i2], innerVertices[i1]}, gatewayFacing.getNormal());
                                //qDebug() << extraVertices[i1] << extraVertices[i2] << innerVertices[i2] << innerVertices[i1] << gatewayFacing;
                                //qDebug() << orientedVertices;
                                faces.push_back(new Face(transcribeVertices(orientedVertices),
                                                         normalIndex(gatewayFacing)
                                                   ));
                                gatewayFacing = gatewayFacing.rotate(facing);
                            }
                        }
                    }
                }
            }
        }
    }

}
