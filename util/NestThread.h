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

    void SetPolygons(const QVector<Polygon> &polygons) {
        m_polygons = polygons;
    }
    void AppendPolygon(const Polygon &aPolygon) {
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
    QVector<Polygon> m_polygons;

    bool doNestWait = false;
    QWaitCondition waitCondition;
    QMutex aMutex;

signals:
    void AddItem(Polygon poly);
    void AddStrip();
    void NestEnd();
};

#endif // NESTTHREAD_H
