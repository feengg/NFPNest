#ifndef STRIP_H
#define STRIP_H

#include <QGraphicsScene>

#include "nest/LB_Polygon2D.h"
using namespace Shape2D;

class Strip : public QGraphicsScene
{
public:
    Strip();

    double getSceneWidth() const;
    double getSceneHeight() const;

    double getStripWidth() const;
    double getStripHeight() const;

    void Reset();
    void InitSize();

    int GetUsedNumber() const;
    double GetUtilization(int index) const;

    QImage DumpToImage();

public slots:
    void AddOneStrip();
    void AddOneItem(LB_Polygon2D poly);

private:
    int stripNb;
    double stripUsed[99]={0};
};

#endif // STRIP_H
