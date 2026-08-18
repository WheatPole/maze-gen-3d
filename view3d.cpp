#include "view3d.h"

View3D::View3D(QWidget *parent)
    : QOpenGLWidget{parent}
{
    setFocusPolicy(Qt::StrongFocus);

    camera = new Camera(2.5, QVector3D(2.0, 0.0, 0.0), QVector3D(0, 0, 0), this);

    updateTimer.start(16, this);
}


void View3D::initializeGL()
{
    initializeOpenGLFunctions();

    logger = new QOpenGLDebugLogger(this);
    logger->initialize();
    QObject::connect(logger, &QOpenGLDebugLogger::messageLogged, this, &View3D::logMessage);
    logger->startLogging();

    glClearColor(0.3, 0.3, 0.3, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    initShaders();

    projection.setToIdentity();
    program.bind();
    program.setUniformValue("projection", projection);
    program.setUniformValue("model", QMatrix4x4());
    program.release();

    skybox.bind();
    skybox.setUniformValue("projection", projection);
    skybox.setUniformValue("model", QMatrix4x4());
    skybox.release();

    initialized = true;
}

void Renderer::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.bind();
    program.setUniformValue("lightPos", QVector4D(camera->position(), 1.0));

    // Calculate model view transformation
    QMatrix4x4 view = camera->getView();
    // Set modelview-projection matrix
    program.setUniformValue("view", view);
    program.setUniformValue("projection", projection);

    //program.setUniformValue("mvp_matrix", projection * view);
    for (auto* terrain : terrains) {
        terrain->drawMesh(&program);
    }

    //program.setUniformValue("texture", 0);
    // engine->drawCubeGeometry(&program);
    program.release();
    skybox.bind();

    skybox.setUniformValue("view", view);
    skybox.setUniformValue("projection", projection);
    tbox->drawMesh(&skybox);

    skybox.release();
}

void Renderer::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    //this->adjustSize();
    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 0.1, zFar = 1500.0, fov = 70.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
    program.bind();
    program.setUniformValue("projection", projection);
    program.release();
    skybox.bind();
    skybox.setUniformValue("projection", projection);
    skybox.release();
}

void Renderer::initShaders() {
    program.bind();
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl")){
        qDebug() << "Error: couldn't compile vertex shader";
        qDebug() << program.log();
        close();
    }

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl")){
        qDebug() << "Error: couldn't compile fragment shader";
        qDebug() << program.log();
        close();
    }

    // Link shader pipeline
    if (!program.link()){
        qDebug() << "Error: couldn't link shaders";
        qDebug() << program.log();
        close();
    }

    // Bind shader pipeline for use
    if (!program.bind()){
        qDebug() << "Error: couldn't bind shaders";
        qDebug() << program.log();
        close();
    }
    program.setUniformValue("sampleColor", QVector4D(0.4, 0.9, 0.2, 1.0));
    program.setUniformValue("lightColor", QVector4D(1.0, 1.0, 1.0, 1.0));
    program.setUniformValue("lightPos", QVector4D(camera->position(), 1.0));
    program.release();

    skybox.bind();
    if (!skybox.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/skybox.vsh")){
        qDebug() << "Error: couldn't compile vertex shader";
        qDebug() << skybox.log();
        close();
    }

    // Compile fragment shader
    if (!skybox.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/skybox.fsh")){
        qDebug() << "Error: couldn't compile fragment shader";
        qDebug() << skybox.log();
        close();
    }

    // Link shader pipeline
    if (!skybox.link()){
        qDebug() << "Error: couldn't link shaders";
        qDebug() << skybox.log();
        close();
    }

    // Bind shader pipeline for use
    if (!skybox.bind()){
        qDebug() << "Error: couldn't bind shaders";
        qDebug() << skybox.log();
        close();
    }
    skybox.release();
}

void Renderer::allocateMeshData(const QByteArray &imageData, QRectF position, const QByteArray &topLayer, int resolution) {
    //QImage image((const unsigned char*)imageData.data(), imageWidth, imageHeight, QImage::Format_ARGB32);
    QImage image;
    image.loadFromData(imageData);
    QImage texture;
    texture.loadFromData(topLayer);
    //image.save("debug.png", "PNG");
    //qDebug() << "Allocating at" << position;
    Terrain* terrainChunk = new Terrain(image, position, resolution, texture, this);
    terrains.append(terrainChunk);
    if (!firstTerrainLoaded) {
        int highestPoint = terrainChunk->getHighestPoint();
        qDebug() << "Highest point: " << highestPoint;
        camera->setY(highestPoint);
    }
    firstTerrainLoaded = true;
}

void Renderer::timerEvent(QTimerEvent *event)
{
    // deltaTime?
    if (event->timerId() == updateTimer.timerId()) {
        if (camera->update())
            update();
    }
}

void Renderer::logMessage(const QOpenGLDebugMessage &message) {
    if (message.severity() != QOpenGLDebugMessage::NotificationSeverity) {
        qDebug() << "[ OpenGLError ]" << message.severity() << message.message();
    }
}
void Renderer::logMessages() {
    const QList<QOpenGLDebugMessage> messages = logger->loggedMessages();
    for (const QOpenGLDebugMessage &message : messages) {
        if (message.severity() != QOpenGLDebugMessage::NotificationSeverity) {
            qDebug() << "[ OpenGLError ]" << message.severity() << message.message();
        }
    }
}

Renderer::~Renderer() {
    makeCurrent();
    for (int i = 0; i < terrains.size(); i++) {
        delete terrains[i];
    }
    delete camera;
    doneCurrent();
}