#ifndef STRIP_H
#define STRIP_H

#include <QGraphicsScene>

#include "nest/LB_Polygon2D.h"
using namespace Shape2D;

const int MaxStripNb = 99;

class Strip : public QGraphicsScene
{
public:
    Strip(double width, double height);

    void setSceneWidth(double width);
    void setSceneHeight(double height);
    double getSceneWidth() const;
    double getSceneHeight() const;

    void setStripWidth(double val);
    void setStripHeight(double val);
    double getStripWidth() const;
    double getStripHeight() const;

    void Reset();

    int GetUsedNumber() const;
    double GetUtilization(int index) const;

    QImage DumpToImage();

public slots:
    void AddOneStrip();
    void AddOneItem(LB_Polygon2D poly);

private:
    double stripWidth;
    double stripHeight;
    int stripNb;
    double stripUsed[MaxStripNb]={0};
};

#endif // STRIP_H
