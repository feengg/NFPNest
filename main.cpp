#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);
    MainWindow aWid;
    aWid.show();
    return app.exec();
}
