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

    const int NumSectors = 100;

    QVector<QVector3D> points;
    QVector<QVector3D> lines;

    for (int i = 0; i < NumSectors; ++i) {
        GLfloat angle = (i * 2 * M_PI) / NumSectors;
        GLfloat angleSin = qSin(angle);
        GLfloat angleCos = qCos(angle);
        points.push_back(QVector3D(0.29f * angleSin, 0.29f * angleCos, -0.05f));
        lines.push_back(QVector3D(0.30f * angleSin, 0.30f * angleCos, -0.05f));
    }

    mapWidget->draw(GL_POINTS, points);
    mapWidget->draw(GL_LINES, lines);

    new ViewerInterfaceAdaptor(mapWidget);
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject("/Viewer", mapWidget);
    connection.registerService(SERVICE_NAME);
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
//    // TODO
//    QVector<QVector3D> lines;

//    lines.push_back(QVector3D(0.04f, 0.04f, -0.05f));
//    lines.push_back(QVector3D(0.06f, 0.04f, -0.05f));

//    lines.push_back(QVector3D(0.06f, 0.06f, -0.05f));
//    lines.push_back(QVector3D(0.06f, 0.04f, -0.05f));

//    lines.push_back(QVector3D(0.06f, 0.06f, -0.05f));
//    lines.push_back(QVector3D(0.04f, 0.06f, -0.05f));

//    lines.push_back(QVector3D(0.04f, 0.04f, -0.05f));
//    lines.push_back(QVector3D(0.04f, 0.06f, -0.05f));

//    mapWidget->draw(GL_LINES, lines);
    mapWidget->cleanup();
}
