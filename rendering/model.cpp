#include "model.h"

Model::Model(Mesh *data, QObject *parent)
    : mesh(data),
    indexBuffer(QOpenGLBuffer::IndexBuffer),
    arrayBuffer(QOpenGLBuffer::VertexBuffer), QObject{parent} {

    refreshData();
}

void Model::initBuffers(QOpenGLFunctions* parent) {
    glFunc = parent;

    arrayBuffer.create();
    indexBuffer.create();
    vao.create();

    bindVertices();
}

void Model::refreshData() {
    vertices.clear();
    indices.clear();

    std::unordered_map<VertexKey, int, VertexHash> indexMap;
    for (Face* face : mesh->faces) {
        // Transcribed indices to model data
        std::array<int, 4> modelFaceIndices;
        for (int i = 0; i < face->indices.size(); i++) {
            int vertexIndex = face->indices[i];
            int normalIndex = face->normal;

            VertexKey key(vertexIndex, normalIndex);
            if (indexMap.find(key) == indexMap.end()) {
                QVector3D vertex = mesh->vertices[vertexIndex-1];
                QVector3D normal = mesh->normals[normalIndex-1];

                vertices.push_back({vertex, normal});
                indexMap[key] = vertices.size()-1;
            }
            modelFaceIndices[i] = indexMap[key];
        }
        int ind[] = { 0, 1, 2,
                     2, 3, 0};

        for (int mInd : ind) {
            auto data = vertices[modelFaceIndices[mInd]];
            indices.push_back(modelFaceIndices[mInd]);
        }
    }
    meshLoaded = true;
}

void Model::bindVertices() {
    if (!arrayBuffer.isCreated()) {
        qDebug("Array buffer not created");
        return;
    }
    if (!indexBuffer.isCreated()) {
        qDebug("Index buffer not created");
        return;
    }
    if (!vao.isCreated()) {
        qDebug("Array array not created");
        return;
    }

    //if (!meshLoaded) return;

    vao.bind();

    arrayBuffer.bind();
    arrayBuffer.allocate(vertices.data(), vertices.size() * sizeof(VertexData));

    indexBuffer.bind();
    indexBuffer.allocate(indices.data(), indices.size() * sizeof(GLuint));

    vao.release();
    arrayBuffer.release();
    indexBuffer.release();

    verticesBound = true;
}

void Model::drawModel(QOpenGLShaderProgram *program) {
    //if (!verticesBound) bindVertices();

    program->bind();

    if (!QOpenGLContext::currentContext()) {
        qWarning() << "No current GL context when trying to draw/allocate!";
        return;
    }

    //texture->bind();
    //program->setUniformValue("texture", 0);
    vao.bind();

    // Tell OpenGL which VBOs to use
    arrayBuffer.bind();
    indexBuffer.bind();

    int locPos = 0;
    int locNormal = 1;
    //int locUV = 2;

    quintptr offset = 0;

    int vertexLocation = program->attributeLocation("a_position");
    if (vertexLocation >= 0) {
        program->enableAttributeArray(vertexLocation);
        program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
    } else {
        qFatal() << "a_position location = -1";
    }

    offset += sizeof(QVector3D);

    int normalLocation = program->attributeLocation("a_normal");
    if (normalLocation >= 0) {
        program->enableAttributeArray(normalLocation);
        program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
    } else {
        qWarning() << "a_normal location = -1";
    }

    /*offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    if (texcoordLocation >= 0) {
        program->enableAttributeArray(texcoordLocation);
        program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
    } else {
        qWarning() << "a_texcoord location = -1";
    }*/

    program->setUniformValue("objectColor", QVector4D(0.8, 0.8, 0.8, 1.0));
    glFunc->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    program->release();
    vao.release();
}