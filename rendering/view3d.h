#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>
#include <QGraphicsView>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QVector2D>
#include <QVector3D>
#include <QList>
#include <QKeyEvent>
#include <QOpenGLDebugLogger>
#include "model.h"
#include "camera.h"

class View3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    View3D(QVector3D cameraPos, QVector3D cameraCentre, QWidget *parent = nullptr);
    void initShaders();
    void appendModel(Model* model);
    inline void updateBuffers() {
        makeCurrent();
        for (auto model : modelList) {
            model->bindVertices();
        }
        doneCurrent();
        update();
    }
    //void drawModel();
    ~View3D();
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void timerEvent(QTimerEvent *event) override;
    inline Camera* getCamera() {
        return camera;
    }
    void logMessage(const QOpenGLDebugMessage &message);
    void logMessages();
    QList<Model*> modelList;
signals:
    void needsRequesting(QPointF position);
private:
    QOpenGLShaderProgram program;
    //Terrain *terrain = nullptr;

    Camera *camera;

    int verticesSize = 0;
    int indicesSize = 0;
    //bool initialized = false;
    QMatrix4x4 projection;
    QOpenGLTexture *texture;
    QBasicTimer updateTimer;

    QOpenGLDebugLogger *logger;
    // first time a terrain chunk has been loaded (for setting the initial camera height)
    bool firstTerrainLoaded = false;
};

#endif // VIEW3D_H
