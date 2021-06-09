#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define GENERATE_RESET 100000
#define MAX_GENERATE_DISTANCE 150
#define MIN_GENERATE_DISTANCE 50
#define GENERATE_ANGLE 120

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mapWidth(1000),
    mapHeight(1000)
{
    ui->setupUi(this);

    qRegisterMetaType<Polygon>("Polygon");
    strip = new Strip(mapWidth,mapHeight);
    nestThread = new NestThread(this);
    connect(nestThread,&NestThread::AddItem,this,&MainWindow::onAddItem);
    connect(nestThread,&NestThread::AddStrip,this,&MainWindow::onAddStrip);
    connect(nestThread,&NestThread::NestEnd,this,&MainWindow::onNestEnd);

    ui->label_stripWidth->setText(tr("Strip width:%1").arg(mapWidth));
    ui->label_stripHeight->setText(tr("Strip height:%1").arg(mapHeight));
    ui->graphicsView_result->setScene(strip);    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_openFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("polygon"),"D:/poly","*.fply");
    if(fileName.isEmpty())
        return;

    srcPolys = loadPolygons(fileName);
    ui->label_polygonNb->setText(tr("Polygon number:%1").arg(srcPolys.size()));
    totalArea = 0;
    foreach(Polygon aPoly,srcPolys) {
        totalArea += abs(aPoly.Area());
    }
    ui->label_totalArea->setText(tr("Total area:%1").arg(totalArea));
    setWindowTitle(tr("NFPNest") + '(' + fileName + ')');
}

void MainWindow::on_action_saveResult_triggered()
{    
}

void MainWindow::on_action_solve_triggered()
{
    nestThread->SetStripSize(mapWidth,mapHeight);

    if(srcPolys.isEmpty())
        return;

    nestThread->SetPolygons(srcPolys);

    nestThread->start();
}

void MainWindow::on_action_reset_triggered()
{
    strip->clear();
    strip->setWidth(mapWidth);
    totalArea = 0;
    srcPolys.clear();

    ui->label_stripNb->setText(tr("Used strip number:XXX"));
    ui->label_polygonNb->setText(tr("Polygon number:XXX"));
    ui->label_totalArea->setText(tr("Total area:XXX"));
    ui->label_utilizationRate->setText(tr("Utilization rate:XXX"));
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

QColor MainWindow::randomColor()
{
    int rand[3] = {0};
    for(int i=0;i<3;++i)
        rand[i] = randInt(0,255);

    return QColor(rand[0],rand[1],rand[2]);
}

Polygon MainWindow::randomPolygon()
{
    int vertexNb = 6;
    int x1, x2;
    int y1, y2;
    int trySize = 0;

    int generateDistance = randInt(MIN_GENERATE_DISTANCE,MAX_GENERATE_DISTANCE);

    Polygon polygon;
    QVector<QLineF> lines;

    x1 = randInt(0,mapWidth / 3);
    y1 = randInt(0,mapHeight / 3);

    x2 = randInt(x1,mapWidth / 3);
    y2 = randInt(0,y1);

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
        x2 = randInt(lastPoint.x() - generateDistance,lastPoint.x() + generateDistance);
        y2 = randInt(lastPoint.y() - generateDistance,lastPoint.y() + generateDistance);
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
                        Polygon poly;
                        return poly;
                    }
                    isIntersect = false;
                    x2 = randInt(lastPoint.x() - generateDistance,lastPoint.x() + generateDistance);
                    y2 = randInt(lastPoint.y() - generateDistance,lastPoint.y() + generateDistance);

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
        Point aPnt(lines.at(ctr).p1().x(),
                   lines.at(ctr).p1().y());
        polygon.push_back(aPnt);
    }
    polygon.push_back(polygon[0]);// append the first vertex to make it close

    return polygon;
}

int MainWindow::randInt(const int &min, const int &max)
{
    return QRandomGenerator::global()->bounded(min,max);
}

QVector<Polygon> MainWindow::loadPolygons(const QString &fileName)
{
    QVector<Polygon> input;

    QFile polyFile(fileName);
    QTextStream aStream(&polyFile);

    if(polyFile.open(QIODevice::ReadOnly))
    {
        while(!aStream.atEnd())
        {
            QString line = aStream.readLine();
            QStringList points = line.split(";");
            Polygon aPoly;
            for(int ctr = 0; ctr < points.size(); ++ctr)
            {
                QStringList coordinates = points[ctr].split(",");
                if(coordinates.size() != 2)
                    continue;

                double x = coordinates[0].toDouble();
                double y = coordinates[1].toDouble();
                aPoly.push_back(Point(x,y));
            }
            aPoly.push_back(aPoly[0]);
            input.push_back(aPoly);
        }
    }

    polyFile.close();
    return input;
}

void MainWindow::test()
{
    for(int i=0;i<5;++i) {
        srcPolys.append(randomPolygon());
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

void MainWindow::onAddItem(Polygon poly)
{
    poly.Translate(poly.ID()*mapWidth,0);
    QPolygonF target = poly.ToPolygonF();
    QGraphicsPolygonItem *anItem = new QGraphicsPolygonItem(target);
    anItem->setPen(QColor(Qt::gray));
    anItem->setBrush(randomColor());
    anItem->setFlags(QGraphicsItem::ItemIsMovable);
    strip->addItem(anItem);
}

void MainWindow::onAddStrip()
{
    int groupSize = nestThread->GetStripNb();
    strip->setWidth(groupSize*mapWidth);
    QGraphicsRectItem *rect = new QGraphicsRectItem(QRectF((groupSize-1)*mapWidth,
                                                           0,
                                                           mapWidth,
                                                           mapHeight));
    rect->setPen(QColor(Qt::red));
    strip->addItem(rect);
}

void MainWindow::onNestEnd()
{
    ui->label_stripNb->setText((tr("Used strip number:%1").arg(nestThread->GetStripNb())));
}
