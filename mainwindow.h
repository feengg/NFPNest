#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRandomGenerator>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QWheelEvent>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QThread>

#include "util/NestThread.h"
#include "Strip.h"

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

    void onAddItem(Polygon poly);
    void onAddStrip();
    void onNestEnd();    

protected:
    void wheelEvent(QWheelEvent *event) override;
    void zoom(int steps);

    QColor randomColor();
    Polygon randomPolygon();
    int randInt(const int &min, const int &max);

private:
    Ui::MainWindow *ui;
    const int mapWidth;
    const int mapHeight;
    double totalArea;

    QVector<Polygon> srcPolys;

    Strip *strip;
    NestThread *nestThread;

    QVector<Polygon> loadPolygons(const QString &fileName);

    void test();
};

#endif // MAINWINDOW_H
