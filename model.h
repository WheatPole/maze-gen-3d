#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include "./parsing/mesh.h"


class Model : public QObject
{
    Q_OBJECT
public:
    Model(Mesh *data, QOpenGLShaderProgram *program = nullptr, QObject *parent = nullptr);
    void initBuffers(QOpenGLFunctions *parent);
    void refreshData();
    void bindVertices();
    void drawModel(QOpenGLShaderProgram *program );

    QOpenGLShaderProgram *m_program;
private:
    struct VertexData
    {
        QVector3D position;
        QVector3D normal;
        //QVector2D texCoord;
    };

    struct VertexKey {
        int vertInd, normalInd;
        VertexKey(int _vertInd, int _normalInd) : vertInd(_vertInd), normalInd(_normalInd) {}
        bool operator ==(const VertexKey &vert) const {
            return (vert.vertInd == vertInd) && (vert.normalInd == normalInd);
        }
    };

    struct VertexHash {
        size_t operator()(const VertexKey& t) const {

            size_t h = (size_t(t.normalInd)<<32)+size_t(t.vertInd);
            h*=1231231557ull; // "random" uneven integer
            h^=(h>>32);
            return h;
        }
    };

    Mesh *mesh;
    //QOpenGLTexture *texture;
    //QOpenGLWidget *_parent;
    QOpenGLBuffer arrayBuffer, indexBuffer;
    QOpenGLVertexArrayObject vao;
    QList<VertexData> vertices;
    QOpenGLFunctions *glFunc;
    QList<GLuint> indices;
};

#endif // MODEL_H
