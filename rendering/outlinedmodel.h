#ifndef OUTLINEDMODEL_H
#define OUTLINEDMODEL_H

#include <QObject>
#include <QWidget>
#include "model.h"

class OutlinedModel : public Model
{
    Q_OBJECT
public:
    explicit OutlinedModel(std::unique_ptr<Mesh> data, QObject *parent = nullptr);
    void initBuffers(QOpenGLFunctions *parent) override;
    void refreshData() override;
    void bindVertices() override;
    void drawModel(QOpenGLShaderProgram *program ) override;

private:

    QOpenGLBuffer outlineIndexBuffer;
    QList<GLuint> outlineIndices;
};

#endif // OUTLINEDMODEL_H
