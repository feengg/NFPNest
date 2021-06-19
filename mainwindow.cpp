#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QDebug>

#include "NestConfigWidget.h"

#define GENERATE_RESET 100000
#define MAX_GENERATE_DISTANCE 150
#define MIN_GENERATE_DISTANCE 50
#define GENERATE_ANGLE 120

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<LB_Polygon2D>("LB_Polygon2D");

    stripScene = new Strip;
    nestThread = new LB_NestThread(this);
    configWid = new NestConfigWidget(this);

    connect(nestThread,&LB_NestThread::AddItem,this,[=](LB_Polygon2D poly) {
        stripScene->AddOneItem(poly);
        arrangNb++;
        ui->progressBar_nest->setValue(100*arrangNb/srcPolys.size());
    });
    connect(nestThread,&LB_NestThread::AddStrip,stripScene,&Strip::AddOneStrip);
    connect(nestThread,&LB_NestThread::NestEnd,this,&MainWindow::onNestEnd);
    connect(configWid,&QDialog::accepted,this,[=]() {
        ui->label_stripWidth->setText(tr("Strip width:%1").arg(stripScene->getStripWidth()));
        ui->label_stripHeight->setText(tr("Strip height:%1").arg(stripScene->getStripHeight()));
        stripScene->Reset();
    });

    ui->graphicsView_result->setScene(stripScene);
    ui->label_stripWidth->setText(tr("Strip width:%1").arg(stripScene->getStripWidth()));
    ui->label_stripHeight->setText(tr("Strip height:%1").arg(stripScene->getStripHeight()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_openFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("polygon"),QApplication::applicationDirPath(),"*.fply");
    if(fileName.isEmpty())
        return;

    srcPolys = loadPolygons(fileName);
    ui->label_polygonNb->setText(tr("Polygon number:%1").arg(srcPolys.size()));

    totalArea = 0;
    foreach(LB_Polygon2D aPoly,srcPolys) {
        totalArea += abs(aPoly.Area());
    }
    ui->label_totalArea->setText(tr("Total area:%1").arg(totalArea));
    setWindowTitle(tr("NFPNest") + '(' + fileName + ')');
}

void MainWindow::on_action_saveResult_triggered()
{
    QString pixName = QFileDialog::getSaveFileName(this,tr("save"),"","*.bmp *.png *.jpg");
    if(pixName.isEmpty())
        return;

    QImage img = stripScene->DumpToImage();
    img.save(pixName);
}

void MainWindow::on_action_solve_triggered()
{
    if(srcPolys.isEmpty())
        return;

    nestThread->SetPolygons(srcPolys);

    nestThread->start();
}

void MainWindow::on_action_reset_triggered()
{
    stripScene->Reset();
    arrangNb = 0;
    totalArea = 0;
    srcPolys.clear();    

    ui->label_stripNb->setText(tr("Used strip number:XXX"));
    ui->label_polygonNb->setText(tr("Polygon number:XXX"));
    ui->label_totalArea->setText(tr("Total area:XXX"));
    ui->label_utilizationRate->setText(tr("Utilization rate:XXX"));
    ui->progressBar_nest->setValue(0);
    setWindowTitle(tr("NFPNest"));
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    zoom(numSteps);
    event->accept();
}

void MainWindow::zoom(int steps)
{
    if(steps < 0)
    {
        ui->graphicsView_result->scale(0.95,0.95);
    }
    else
    {
        ui->graphicsView_result->scale(1.05,1.05);
    }
}

void MainWindow::on_action_test_triggered()
{
    test();
}

void MainWindow::on_action_pause_triggered()
{
    nestThread->PauseNest();
}

void MainWindow::on_action_resume_triggered()
{
    nestThread->ResumeNest();
}

void MainWindow::on_action_setting_triggered()
{
    configWid->show();
}

void MainWindow::onNestEnd()
{
    int stripNb = stripScene->GetUsedNumber();
    ui->label_stripNb->setText((tr("Used strip number:%1").arg(stripNb)));
    // Statistics
    double anArea = 0;
    ui->label_utilizationRate->clear();
    QString content;
    for(int i=0;i<stripNb;++i) {
        anArea = stripScene->GetUtilization(i);
        content.append(tr("Strip %1 Utilization rate:%2 % \n").arg(i).arg(100*anArea/(stripScene->getStripWidth()*stripScene->getStripHeight())));
    }
    ui->label_utilizationRate->setText(content);
}

LB_Polygon2D MainWindow::randomPolygon()
{
    int vertexNb = 6;
    int x1, x2;
    int y1, y2;
    int trySize = 0;

    int generateDistance = RandInt(MIN_GENERATE_DISTANCE,MAX_GENERATE_DISTANCE);

    LB_Polygon2D polygon;
    QVector<QLineF> lines;

    x1 = RandInt(0,stripScene->getStripWidth() / 3);
    y1 = RandInt(0,stripScene->getStripHeight() / 3);

    x2 = RandInt(x1,stripScene->getStripWidth() / 3);
    y2 = RandInt(0,y1);

    QLineF line;
    line.setP1(QPointF(x1,y1));
    line.setP2(QPointF(x2,y2));
    lines.append(line);

    QPointF lastPoint = lines.last().p2();
    line.setP1(lastPoint);

    qreal angle = 0;
    QLineF newLine;
    qreal newAngle = 0;
    qreal firstAngle = 0;

    do
    {
        x2 = RandInt(lastPoint.x() - generateDistance,lastPoint.x() + generateDistance);
        y2 = RandInt(lastPoint.y() - generateDistance,lastPoint.y() + generateDistance);
        line.setP2(QPointF(x2,y2));

        angle = line.angleTo(lines.last());

    } while(angle < 0 || angle > GENERATE_ANGLE);
    lines.append(line);

    for(int ctr = 2; ctr < vertexNb; ++ctr)
    {
        trySize = 0;

        lastPoint = lines.last().p2();
        line.setP1(lastPoint);
        do
        {
            angle = 0;
            newAngle = 0;
            firstAngle = 0;
            do
            {
                bool isIntersect = false;
                do
                {
                    ++trySize;
                    if(trySize > GENERATE_RESET)
                    {
                        LB_Polygon2D poly;
                        return poly;
                    }
                    isIntersect = false;
                    x2 = RandInt(lastPoint.x() - generateDistance,lastPoint.x() + generateDistance);
                    y2 = RandInt(lastPoint.y() - generateDistance,lastPoint.y() + generateDistance);

                    line.setP2(QPointF(x2,y2));
                    for(int ctr2 = 0; ctr2 < lines.size()-1; ++ctr2)
                    {
                        QPointF *pnt = new QPointF;
                        if(line.intersects(lines.at(ctr2),pnt) == QLineF::BoundedIntersection)
                        {
                            isIntersect = true;
                            break;
                        }
                    }
                } while(isIntersect);
                angle = line.angleTo(lines.last());

            } while(angle < 0 || angle > GENERATE_ANGLE);

            newLine.setPoints(line.p2(),lines.first().p1());
            newAngle = newLine.angleTo(lines.last());
            firstAngle = lines.first().angleTo(lines.last());

        } while(newAngle < angle || newAngle > firstAngle);
        lines.append(line);
    }

    for(int ctr = 0; ctr < vertexNb; ++ctr)
    {
        LB_Coord2D aPnt(lines.at(ctr).p1().x(),
                   lines.at(ctr).p1().y());
        polygon.push_back(aPnt);
    }    

    return polygon;
}

QVector<LB_Polygon2D> MainWindow::loadPolygons(const QString &fileName)
{
    QVector<LB_Polygon2D> input;

    QFile polyFile(fileName);
    QTextStream aStream(&polyFile);

    if(polyFile.open(QIODevice::ReadOnly))
    {
        while(!aStream.atEnd())
        {
            QString line = aStream.readLine();
            QStringList points = line.split(";");
            LB_Polygon2D aPoly;
            for(int ctr = 0; ctr < points.size(); ++ctr)
            {
                QStringList coordinates = points[ctr].split(",");
                if(coordinates.size() != 2)
                    continue;

                double x = coordinates[0].toDouble();
                double y = coordinates[1].toDouble();
                aPoly.push_back(LB_Coord2D(x,y));
            }
            input.push_back(aPoly);
        }
    }

    polyFile.close();
    return input;
}

void MainWindow::test()
{
    for (int i=0;i<100;++i) {
        srcPolys.append(randomPolygon());
    }
}
