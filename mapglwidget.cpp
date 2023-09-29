/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mapglwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>


static const char *vertexShaderSourceCore =
    "#version 150\n"
    "in vec4 vertex;\n"
    "in vec3 normal;\n"
    "out vec3 vert;\n"
    "out vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 150\n"
    "in highp vec3 vert;\n"
    "in highp vec3 vertNormal;\n"
    "out highp vec4 fragColor;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   fragColor = vec4(col, 1.0);\n"
    "}\n";

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

bool MapGLWidget::transparent = false;

MapGLWidget::MapGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    if (transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

MapGLWidget::~MapGLWidget()
{
    cleanup();
}

QSize MapGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MapGLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void MapGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void MapGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void MapGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void MapGLWidget::cleanup()
{
    if (shaderProgram == nullptr)
        return;
    makeCurrent();

    points.vbo.destroy();
    lines.vbo.destroy();

    delete shaderProgram;
    shaderProgram = nullptr;
    doneCurrent();
}

int MapGLWidget::draw(GLenum mode, QVector<QVector3D> &data)
{
    drawMode = mode;

    for(int i = 0; i <= data.size()-6; i++){
//        QVector3D n = QVector3D::normal(QVector3D(x3 - x1, y3 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));
        add(mode, data.at(i), QVector3D(0, 0, -0.05f));
    }
    return 0;
}

void MapGLWidget::add(GLenum mode, const QVector3D &v, const QVector3D &n)
{
    switch(mode){

    case GL_POINTS:
        points.add(v, n);
        break;

    case GL_LINES:
        lines.add(v, n);
        break;

    default:
        return;
    }
}

void MapGLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &MapGLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, transparent ? 0 : 1); // задаём красные, зеленые, синие и альфа-значения, используемые glClear для очистки цветовых буферов.

    shaderProgram = new QOpenGLShaderProgram; // создаём шейдерную программу
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, core ? vertexShaderSourceCore : vertexShaderSource); // добавляем программу вершинного шейдера
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, core ? fragmentShaderSourceCore : fragmentShaderSource); // добавляем программу фрагментного шейдера
    shaderProgram->bindAttributeLocation("vertex", 0);
    shaderProgram->bindAttributeLocation("normal", 1);
    shaderProgram->link(); // связываем шейдеры вместе

    shaderProgram->bind(); // связывает эту шейдерную программу с активным QOpenGLContext и делает ее текущей шейдерной программой.
    projMatrixLoc = shaderProgram->uniformLocation("projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("mvMatrix");
    normalMatrixLoc = shaderProgram->uniformLocation("normalMatrix");
    lightPosLoc = shaderProgram->uniformLocation("lightPos");

    points.vao.create();
    QOpenGLVertexArrayObject::Binder pointsVaoBinder(&(points.vao));
    points.initVbo();
    setupVertexAttribs(points.vbo);

    lines.vao.create();
    QOpenGLVertexArrayObject::Binder linesVaoBinder(&(lines.vao));
    lines.initVbo();
    setupVertexAttribs(lines.vbo);

    // Our camera never changes in this example.
    camera.setToIdentity();
    camera.translate(0, 0, -1);

    // Light position is fixed.
    shaderProgram->setUniformValue(lightPosLoc, QVector3D(0, 0, 70));

    shaderProgram->release();
}

void MapGLWidget::setupVertexAttribs(QOpenGLBuffer &vbo)
{
    vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                             nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    vbo.release();
}

void MapGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); // Enabling depth test for added fragments
    glEnable(GL_CULL_FACE); // Don't drawing invisible fragments

    world.setToIdentity(); // Init E matrix(Identity matrix) in m_world
    world.rotate(180.0f - (xRot / 16.0f), 1, 0, 0);
    world.rotate(yRot / 16.0f, 0, 1, 0);
    world.rotate(zRot / 16.0f, 0, 0, 1);

    shaderProgram->bind();
    shaderProgram->setUniformValue(projMatrixLoc, proj);
    shaderProgram->setUniformValue(mvMatrixLoc, camera * world);
    QMatrix3x3 normalMatrix = world.normalMatrix();
    shaderProgram->setUniformValue(normalMatrixLoc, normalMatrix);

    QOpenGLVertexArrayObject::Binder pointsVaoBinder(&(points.vao));
    glDrawArrays(GL_POINTS, 0, points.vertexCount());

    QOpenGLVertexArrayObject::Binder linesVaoBinder(&(lines.vao));
    glDrawArrays(GL_LINES, 0, lines.vertexCount());

    shaderProgram->release();
}

void MapGLWidget::resizeGL(int w, int h)
{
    proj.setToIdentity();
    proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void MapGLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void MapGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int mouseSpeed = 8;

    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot - mouseSpeed * dy);
        setYRotation(yRot - mouseSpeed * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot - mouseSpeed * dy);
        setZRotation(zRot - mouseSpeed * dx);
    } else if(event->buttons() & Qt::MiddleButton){
        // TODO make moving

//        camera.translate(1.0 / dx, 1.0 / dy);
    }
    m_lastPos = event->pos();
}

void MapGLWidget::wheelEvent(QWheelEvent *event)
{
    int dx = event->angleDelta().x();
    int dy = event->angleDelta().y();
    // TODO make scaling
}
