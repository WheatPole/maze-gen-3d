#ifndef VERTEXBOX_H
#define VERTEXBOX_H

#include "../engine/tile.h"

class VertexBox {
public:
    QVector3D vertices[8 + 4 * 6];
    int size = 0;

    VertexBox(TileBox &tBox) {
        // in theory this should be tBox.getQuad(YNEG) for the bottom quad
        // but it shouldn't matter since we're providing normals anyway
        Quad bottom = tBox.bottomQuad;

        QVector3D up = tBox.up;
        addQuad(bottom);
        Quad top = bottom.addToOrigin(up);
        addQuad(top);

        for (Quad inner : tBox.innerQuads) {
            addQuad(inner);
        }
    }

    QVector3D at(int index) const {
        return vertices[index];
    }

    inline bool addQuad(Quad quad) {
        vertices[size++] = quad.origin;
        vertices[size++] = quad.origin + quad.right;
        vertices[size++] = quad.origin + quad.right + quad.up;
        vertices[size++] = quad.origin + quad.up;
        return true;
    }
    // Get outer wall indices
    std::array<int, 4> getOWIndices(WallFacing facing) const {
        // oriented such that it starts from the origin point and follow the general right/up vector principle
        switch (facing.facing) {
            case Facing::YNEG: return { 0, 3, 2, 1};
            case Facing::YPOS: return{ 4, 5, 6, 7};
            case Facing::XNEG: return{ 0, 1, 5, 4};
            case Facing::XPOS: return{ 3, 7, 6, 2};
            case Facing::ZNEG: return{ 0, 4, 7, 3};
            case Facing::ZPOS: return{ 1, 2, 6, 5};
        }
        return {0,0,0,0};
    }

    std::array<QVector3D, 4> getOWVertices(WallFacing facing) const {
        std::array<int, 4> ind = getOWIndices(facing);
        std::array<QVector3D, 4> res;
        for (int i = 0; i < 4; res[i] = vertices[ind[i]], i++);
        return res;
    }

    // Get inner wall indices
    std::array<int, 4> getIWIndices(WallFacing facing) const {

        switch (facing.facing) {
            case Facing::XNEG: return{ 8, 9, 10, 11}; ;
            case Facing::XPOS: return{ 12, 13, 14, 15}; ;
            case Facing::YNEG: return{ 16, 17, 18, 19}; ;
            case Facing::YPOS: return{ 20, 21, 22, 23}; ;
            case Facing::ZNEG: return{ 24, 25, 26, 27}; ;
            case Facing::ZPOS: return{ 28, 29, 30, 31}; ;
        }
            return {0,0,0,0};
    }

    std::array<QVector3D, 4> getIWVertices(WallFacing facing) const {
            std::array<int, 4> ind = getIWIndices(facing);
            std::array<QVector3D, 4> res;
            for (int i = 0; i < 4; res[i] = vertices[ind[i]], i++);
            return res;
    }
};

#endif // VERTEXBOX_H
