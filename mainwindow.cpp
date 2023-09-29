#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QKeyEvent>
#include <qmath.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mapWidget = new MapGLWidget(this);
    setCentralWidget(mapWidget);

//    QVBoxLayout *mainLayout = new QVBoxLayout;
//    QHBoxLayout *container = new QHBoxLayout;
//    container->addWidget(mapWidget);

//    QWidget *w = new QWidget;
//    w->setLayout(container);
//    mainLayout->addWidget(w);

//    setLayout(mainLayout);
    setWindowTitle(tr("Map viewer"));

    const int NumSectors = 10;

    QVector<QVector3D> points;
    QVector<QVector3D> lines;

    lines.push_back(QVector3D(0.30f, 0.30f, -0.05f));
    lines.push_back(QVector3D(0.05f, 0.05f, -0.05f));
    lines.push_back(QVector3D(0.55f, 0.55f, -0.05f));

    lines.push_back(QVector3D(0.40f, 0.40f, -0.05f));
    lines.push_back(QVector3D(0.15f, 0.15f, -0.05f));
    lines.push_back(QVector3D(0.65f, 0.65f, -0.05f));

    for (int i = 0; i < NumSectors; ++i) {
        GLfloat angle = (i * 2 * M_PI) / NumSectors;
        GLfloat angleSin = qSin(angle);
        GLfloat angleCos = qCos(angle);
        points.push_back(QVector3D(0.30f * angleSin, 0.30f * angleCos, -0.05f));
    }

    mapWidget->draw(GL_POINTS, lines);
    mapWidget->draw(GL_LINES, points);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void MainWindow::on_action_Save_triggered()
{
    // TODO
}

void MainWindow::on_action_Open_triggered()
{
    // TODO
}

void MainWindow::on_action_Quit_triggered()
{
    // TODO
}

void MainWindow::on_action_About_triggered()
{
    // TODO
}
