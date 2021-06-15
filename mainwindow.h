#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>

#include "nest/LB_NestThread.h"
#include "Strip.h"
#include "NestConfigWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_openFile_triggered();
    void on_action_saveResult_triggered();
    void on_action_solve_triggered();
    void on_action_reset_triggered();
    void on_action_test_triggered();
    void on_action_pause_triggered();
    void on_action_resume_triggered();
    void on_action_setting_triggered();

    void onNestEnd();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void zoom(int steps);       

private:
    Ui::MainWindow *ui;

    double totalArea;    

    QVector<LB_Polygon2D> srcPolys;

    Strip *stripScene;
    LB_NestThread *nestThread;
    NestConfigWidget *configWid;

    LB_Polygon2D randomPolygon();

    QVector<LB_Polygon2D> loadPolygons(const QString &fileName);

    void test();
};

#endif // MAINWINDOW_H
