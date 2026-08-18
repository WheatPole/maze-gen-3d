#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "./parsing/mesh.h"

struct VertexData
{
    QVector3D position;
    QVector3D normal;
    QVector2D texCoord;
};

class Model : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Model(Mesh *data, QOpenGLShaderProgram *program);
    void drawModel();
private:
    Mesh *mesh;
    QOpenGLShaderProgram *m_program;
    //QOpenGLTexture *texture;
    //QOpenGLWidget *_parent;
    QOpenGLBuffer arrayBuffer, indexBuffer;
    QOpenGLVertexArrayObject vao;
    VertexData *vertices;
    GLuint *indices;
};

#endif // MODEL_H
