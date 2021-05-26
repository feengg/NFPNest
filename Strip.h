#ifndef STRIP_H
#define STRIP_H

#include <QGraphicsScene>

class Strip : public QGraphicsScene
{
public:
    Strip(double width, double height);

    void setWidth(double width);
    void setHeight(double height);
    double getWidth();
    double getHeight();
};

#endif // STRIP_H
