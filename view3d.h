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
#include "./parsing/mesh.h"
#include "camera.h"

class View3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    View3D(QWidget *parent = nullptr);
    void initShaders();
    // Resolution is the amount of squares per row or column
    void allocateMeshData(const QByteArray &imageData, QRectF position, const QByteArray &topLayer, int resolution = 100);
    void drawMesh();
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
signals:
    void needsRequesting(QPointF position);
private:
    QOpenGLShaderProgram program;
    //Terrain *terrain = nullptr;

    Camera *camera;
    QList<Mesh*> meshList;

    int verticesSize = 0;
    int indicesSize = 0;
    bool initialized = false;
    QMatrix4x4 projection;
    QOpenGLTexture *texture;
    QBasicTimer updateTimer;

    QOpenGLDebugLogger *logger;
    // first time a terrain chunk has been loaded (for setting the initial camera height)
    bool firstTerrainLoaded = false;
};

#endif // VIEW3D_H
