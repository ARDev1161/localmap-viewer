#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mapglwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    MapGLWidget *mapWidget;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_action_Save_triggered();
    void on_action_Open_triggered();
    void on_action_Quit_triggered();
    void on_action_About_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
