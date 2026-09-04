#ifndef OUTLINEDMODEL_H
#define OUTLINEDMODEL_H

#include <QObject>
#include <QWidget>
#include "model.h"

class OutlinedModel : public Model
{
    Q_OBJECT
public:
    explicit OutlinedModel(Mesh *data, QObject *parent = nullptr);
    void initBuffers(QOpenGLFunctions *parent) override;
    void refreshData() override;
    void bindVertices() override;
    void drawModel(QOpenGLShaderProgram *program ) override;

signals:

private:

    QOpenGLBuffer outlineIndexBuffer;
    QList<GLuint> outlineIndices;
};

#endif // OUTLINEDMODEL_H
