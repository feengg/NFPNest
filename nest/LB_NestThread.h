#ifndef LB_NESTTHREAD_H
#define LB_NESTTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include "LB_NFPHandle.h"
using namespace NFPHandle;
using namespace Shape2D;

class LB_NestThread : public QThread
{
    Q_OBJECT
public:
    explicit LB_NestThread(QObject *parent);

    void run() override;

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
    void RotateToMinBounds();

private:
    QVector<LB_Polygon2D> polygons;

    bool doNestWait = false;
    QWaitCondition waitCondition;
    QMutex aMutex;

signals:
    void AddItem(LB_Polygon2D poly);
    void AddStrip();
    void NestEnd();
};

#endif // LB_NESTTHREAD_H
