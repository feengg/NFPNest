#ifndef NESTTHREAD_H
#define NESTTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include "nest/LB_NFPHandle.h"
using namespace LB_NFP;
using namespace Shape2D;

class NestThread : public QThread
{
    Q_OBJECT
public:
    explicit NestThread(QObject *parent);

    void run() override;

    void SetStripSize(double width, double height);

    void SetPolygons(const QVector<LB_Polygon2D> &polygonVec) {
        polygons = polygonVec;
    }
    void AppendPolygon(const LB_Polygon2D &aPolygon) {
        polygons.push_back(aPolygon);
    }

    void PauseNest();
    void ResumeNest();

protected:
    void SortByWidthDecreasing();
    void SortByAreaDecreasing();

private:
    double stripWidth;
    double stripHeight;
    QVector<LB_Polygon2D> polygons;

    bool doNestWait = false;
    QWaitCondition waitCondition;
    QMutex aMutex;

signals:
    void AddItem(LB_Polygon2D poly);
    void AddStrip();
    void NestEnd();
};

#endif // NESTTHREAD_H
