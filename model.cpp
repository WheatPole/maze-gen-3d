#include "model.h"

Model::Model(Mesh *data, QOpenGLShaderProgram *program)
    : mesh(data), m_program(program), indexBuffer(QOpenGLBuffer::IndexBuffer), arrayBuffer(QOpenGLBuffer::VertexBuffer) {
    //vertices.reserve((mesh->innerSize))
}

void Model::refreshData() {
    vertices.clear();
    indices.clear();

    struct VertexKey {
        int vertInd, normalInd;
        VertexKey(int _vertInd, int _normalInd) : vertInd(_vertInd), normalInd(_normalInd) {}
    };

    struct VertexHash {
        size_t operator()(const VertexKey& t) const {

            size_t h = (size_t(t.normalInd)<<32)+size_t(t.vertInd);
            h*=1231231557ull; // "random" uneven integer
            h^=(h>>32);
            return h;
        }
    };

    std::unordered_map<VertexKey, int, VertexHash> indexMap;
    for (Face* face : mesh->faces) {
        std::array<int, 4> modelVertices, modelIndices;
        for (int i = 0; i < face->indices.size(); i++) {
            int vertexIndex = face->indices[i];
            VertexKey key(vertexIndex, face->normal);
            if (indexMap.find(key) != indexMap.end()) {

            }
            else {

            }
        }

        VertexData vData1, vData2;
    }
}