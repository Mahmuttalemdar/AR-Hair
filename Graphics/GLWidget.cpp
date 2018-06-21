#include "GLWidget.h"
#include <QPainter>
#include <QResizeEvent>
#include <QGraphicsDropShadowEffect>

bool GLWidget::_transparent = false;

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent) {
    _background = new BackgroundObject();
    _hair = new TransformedObject(HAIR, 1);
    _head = new TransformedObject(HEAD);
    _renderHair = true;

    if (_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

GLWidget::~GLWidget() {
    delete _background;
    delete _hair;
    delete _head;
}

void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _background->init();
    _head->init();
    _hair->init();
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, int(!_transparent));

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    _background->render();
    f->glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    _head->render();
    f->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if (_renderHair)
        _hair->render();
}

void GLWidget::resizeGL(int width, int height) {
    _hair->updateProjectionMatrix(GLfloat(width) / height);
    _head->updateProjectionMatrix(GLfloat(width) / height);
}

void GLWidget::updateBackgroundImage(const QImage &image) {
    _background->updateTexture(image);
    this->update();
}

void GLWidget::updatePosition(cv::Mat &rmat, cv::Mat &tvec) {
    _hair->updatePosition(rmat, tvec);
    _head->updatePosition(rmat, tvec);
}

void GLWidget::cleanup() {
    GLWidget::~GLWidget();
}

void GLWidget::nextHair() {
    _renderHair = false;
    _hair->incIdx();
    _hair->reinit();
    _renderHair = true;
}

void GLWidget::prevHair() {
    _renderHair = false;
    _hair->decIdx();
    _hair->reinit();
    _renderHair = true;
}
