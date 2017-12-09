#include "GLWidget.h"
#include <QPainter>
#include <QResizeEvent>

bool GLWidget::_transparent = false;

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent) {
    _background = new BackgroundRendering();
    _hair = new HairRendering();
    if (_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

GLWidget::~GLWidget() {
    delete _background;
    delete _hair;
}

void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _background->init();
    _hair->init();
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, int(!_transparent));

    _background->render();
    _hair->render();
}

void GLWidget::resizeGL(int width, int height) {
    _hair->updateProjectionMatrix(GLfloat(width) / height);
}

void GLWidget::updateBackgroundImage(const QImage &image) {
    _background->updateTexture(image);
    this->update();
}

void GLWidget::updatePosition(cv::Mat &rmat, cv::Mat &tvec) {
    _hair->updatePosition(rmat, tvec);
}

void GLWidget::cleanup()
{
    GLWidget::~GLWidget();
}
