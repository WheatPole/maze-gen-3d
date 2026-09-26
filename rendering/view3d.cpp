#include "view3d.h"

View3D::View3D(QVector3D cameraPos, QVector3D cameraCentre, QWidget *parent)
    : QOpenGLWidget{parent}
{
    setFocusPolicy(Qt::StrongFocus);
    this->setMinimumSize(500, 500);

    this->installEventFilter(this);

    camera = new Camera(2.5, cameraPos, cameraCentre, true, this);
    clickHandler = new InteractionHandler(camera, this);

    QObject::connect(camera, &Camera::cameraPositionChanged, [&] (QVector3D cameraPos) {
        this->update();
    });

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
    //glEnable(GL_CULL_FACE);

    initShaders();

    program.bind();
    program.setUniformValue("projection", QMatrix4x4());
    program.setUniformValue("model", QMatrix4x4());
    program.release();
    for (Model *model : modelList) {
        model->initBuffers(this);
    }
}

void View3D::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.bind();
    program.setUniformValue("lightPos", QVector4D(camera->position(), 1.0));

    // Calculate model view transformation
    QMatrix4x4 view = camera->getView();
    // Set modelview-projection matrix
    program.setUniformValue("view", view);
    program.setUniformValue("projection", camera->getProjection());

    //program.setUniformValue("mvp_matrix", projection * view);
    for (Model *model : modelList) {
        model->drawModel(&program);
    }

    program.release();
}

void View3D::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);

    program.bind();
    program.setUniformValue("projection", camera->getProjection());
    program.release();
}

bool View3D::eventFilter(QObject* object, QEvent* event) {
    //qDebug() << "Recieved event of type " << event->type() << event;
    if (event->type() == QEvent::KeyPress) {
        return camera->handleKeyPress(static_cast<QKeyEvent*>(event));
    }
    else if (event->type() == QEvent::KeyRelease) {
        return camera->handleKeyRelease(static_cast<QKeyEvent*>(event));
    }
    else if (event->type() == QEvent::MouseButtonPress) {
        return camera->handleMousePress(static_cast<QMouseEvent*>(event));
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        camera->handleMouseRelease(static_cast<QMouseEvent*>(event));
        clickHandler->handleMouseRelease(static_cast<QMouseEvent*>(event));
    }
    else if (event->type() == QEvent::Wheel) {
        return camera->handleWheel(static_cast<QWheelEvent*>(event));
    }
    else if (event->type() == QEvent::MouseMove) {
        return camera->handleMouseMove(static_cast<QMouseEvent*>(event));

    }
    return false;
}

void View3D::initShaders() {
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
    program.setUniformValue("lightColor", QVector4D(1.0, 1.0, 1.0, 1.0));
    program.setUniformValue("lightPos", QVector4D(camera->position(), 1.0));
    program.setUniformValue("selectionColor", QVector4D(0.35, 0.85, 0.35, 0.5));

    program.release();
}

void View3D::appendModel(Model *model) {
    modelList.append(model);
    if (this->isValid()) {
        makeCurrent();
        model->initBuffers(this);
        doneCurrent();
    }
    else {
        qDebug() << "Context not yet initialized";
    }
}

void View3D::timerEvent(QTimerEvent *event)
{
    // deltaTime?
    if (event->timerId() == updateTimer.timerId()) {
        if (camera->update())
            update();
    }
}

void View3D::logMessage(const QOpenGLDebugMessage &message) {
    if (message.severity() != QOpenGLDebugMessage::NotificationSeverity) {
        qDebug() << "[ OpenGLError ]" << message.severity() << message.message();
    }
}
void View3D::logMessages() {
    const QList<QOpenGLDebugMessage> messages = logger->loggedMessages();
    for (const QOpenGLDebugMessage &message : messages) {
        if (message.severity() != QOpenGLDebugMessage::NotificationSeverity) {
            qDebug() << "[ OpenGLError ]" << message.severity() << message.message();
        }
    }
}

void View3D::setSelection(Selection sel) {
    selection = sel;

    makeCurrent();
    program.bind();
    program.setUniformValue("selectionStart", QVector4D(sel.origin));
    program.setUniformValue("selectionEnd", QVector4D(sel.origin + sel.size));
    program.release();
    doneCurrent();
}

View3D::~View3D() {
    makeCurrent();
    delete camera;
    doneCurrent();
}
