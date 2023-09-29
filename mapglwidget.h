#ifndef MAPGLWIDGET_H
#define MAPGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

#include <qopengl.h>
#include <QVector>
#include <QVector3D>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

struct Buffer{
    QVector<GLfloat> vertex;
    long counter = 0;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;

    void initVbo(){
        vbo.create();
        vbo.bind();
        vbo.allocate( vertex.constData(), counter * sizeof(GLfloat));
    }

    void add(const QVector3D &v, const QVector3D &n)
    {
        vertex.push_back(v.x());
        vertex.push_back(v.y());
        vertex.push_back(v.z());

        vertex.push_back(n.x());
        vertex.push_back(n.y());
        vertex.push_back(n.z());

        counter += 6;
    }

    long vertexCount() const { return counter / 6; }

    const GLfloat* constData(){ return vertex.constData(); }
};

class MapGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    bool core;
    int xRot = 0;
    int yRot = 0;
    int zRot = 0;
    QPoint m_lastPos;

    Buffer points;
    Buffer lines;

    QOpenGLShaderProgram *shaderProgram = nullptr;
    int projMatrixLoc = 0;
    int mvMatrixLoc = 0;
    int normalMatrixLoc = 0;
    int lightPosLoc = 0;
    QMatrix4x4 proj;
    QMatrix4x4 camera;
    QMatrix4x4 world;
    static bool transparent;

    unsigned int drawMode = GL_POINTS;

    void setupVertexAttribs(QOpenGLBuffer &vbo);

    void add(GLenum mode, const QVector3D &v, const QVector3D &n);
public:
    MapGLWidget(QWidget *parent = nullptr);
    ~MapGLWidget();

    static bool isTransparent() { return transparent; }
    static void setTransparent(bool t) { transparent = t; }

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    int draw(GLenum mode, QVector<QVector3D> &data);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void cleanup();

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
};


#endif
