#ifndef MESH_H
#define MESH_H

#include "../engine/tile.h"
#include "../engine/generator.h"
#include "vertexbox.h"
#include "../util/triplet.h"
#include <map>
#include <QString>
#include <QHash>

struct Face {
    std::array<int, 4> indices;
    int normal;
    Face(std::array<int, 4> _indices, int _normal) : indices(_indices), normal(_normal) {}
};

class Mesh {
private:
    struct hash {
        size_t operator()(const QVector3D& t) const {
            return qHashMulti(0, t.x(), t.y(), t.z());
        }
    };
    int maxSize, m_size;

public:
    Generator *generator;
    std::vector<VertexBox*> vBoxList;
    std::vector<QVector3D> vertices;
    std::unordered_map<QVector3D, int, hash> vertexHash;

    // Returns the size of currently appended vertices
    inline int size() const {
        return m_size;
    }

    std::vector<QVector3D> normals = {
        WallFacing::getNormal(WallFacing::XNEG),
        WallFacing::getNormal(WallFacing::XPOS),
        WallFacing::getNormal(WallFacing::YNEG),
        WallFacing::getNormal(WallFacing::YPOS),
        WallFacing::getNormal(WallFacing::ZNEG),
        WallFacing::getNormal(WallFacing::ZPOS),
    };

    // First should go the inner faces, then the outer faces
    std::vector<Face*> faces;

    Mesh(Generator *generator);

    inline void refresh() {
        vertexHash.clear();

        faces.clear();
        vertices.clear();

        m_size = 0;
        fillInnerVertices();
        fillOuterVertices();
    }

    void updateVbox();

private:
    void fillInnerVertices();
    void fillOuterVertices();

    inline int normalIndex(WallFacing facing) const {
        switch (facing.facing) {
        case Facing::XNEG: return 1;
        case Facing::XPOS: return 2;
        case Facing::YNEG: return 3;
        case Facing::YPOS: return 4;
        case Facing::ZNEG: return 5;
        case Facing::ZPOS: return 6;
        }
        return 1;
    }

    void roundVec(QVector3D &vec, int dec) {
        long mul = 1;
        for (int i = 0; i < dec; i++) mul *= 10;
        vec.setX(((long)(vec.x() * mul)) / (float)mul);
        vec.setY(((long)(vec.y() * mul)) / (float)mul);
        vec.setZ(((long)(vec.z() * mul)) / (float)mul);
    }

    // TODO: look into why there are STILL duplicates on edges
    // Converts vertices to Wavefront obj-like face vertices and return indices
    std::array<int, 4> transcribeVertices(std::array<QVector3D, 4> vertexData) {
        std::array<int, 4> result;
        for (int i = 0; i < vertexData.size(); i++) {
            QVector3D vec = vertexData[i];
            roundVec(vec, 4);
            if (vertexHash.find(vec) == vertexHash.end()) {
                vertices.push_back(vec);
                vertexHash[vec] = m_size++;
            }
            result[i] = vertexHash[vec] + 1;
        }
        return result;
    }

    // Returns the index of the indices list of the vertex box, where vector "target" is on the
    // line facing towards the "facing" direction (so only 2 axes are equal, and not
    // the "facing" axis)
    int lookupVertex(WallFacing facing, std::array<int, 4> list, QVector3D target, VertexBox &box) {
        //auto vert = box.vertices;
        for (int i = 0; i < list.size(); i++) {
            bool eqX = abs( box.at(list[i]).x() - target.x() ) < 1e-5;
            bool eqY = abs( box.at(list[i]).y() - target.y() ) < 1e-5;
            bool eqZ = abs( box.at(list[i]).z() - target.z() ) < 1e-5;

            switch (facing.facing) {
            case Facing::XPOS:
            case Facing::XNEG:
                if (eqZ && eqY) return i;
                break;
            case Facing::YPOS:
            case Facing::YNEG:
                if (eqX && eqZ) return i;
                break;
            case Facing::ZPOS:
            case Facing::ZNEG:
                if (eqX && eqY) return i;
                break;
            }
        }
        return -1;
    }
};

#endif // MESH_H
