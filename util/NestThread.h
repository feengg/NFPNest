#ifndef NESTTHREAD_H
#define NESTTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include "LB_NFPHandle.h"
using namespace LB_NFP;
using namespace Shape2D;

class NestThread : public QThread
{
    Q_OBJECT
public:
    explicit NestThread(QObject *parent);

    void run() override;

    void SetStripSize(double width, double height);

    void SetPolygons(const QVector<LB_Polygon2D> &polygons) {
        m_polygons = polygons;
    }
    void AppendPolygon(const LB_Polygon2D &aPolygon) {
        m_polygons.push_back(aPolygon);
    }

    int GetStripNb() const {
        return m_stripNb;
    }

    void PauseNest();
    void ResumeNest();

protected:
    void SortByWidthDecreasing();
    void SortByAreaDecreasing();

private:
    double m_stripWidth;
    double m_stripHeight;
    int m_stripNb;
    QVector<LB_Polygon2D> m_polygons;

    bool doNestWait = false;
    QWaitCondition waitCondition;
    QMutex aMutex;

signals:
    void AddItem(LB_Polygon2D poly);
    void AddStrip();
    void NestEnd();
};

#endif // NESTTHREAD_H
