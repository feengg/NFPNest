#include "Strip.h"

Strip::Strip(double width, double height)
{
    this->setSceneRect(0,0,width,height);
}

void Strip::setWidth(double width)
{
    double height = this->sceneRect().height();

    this->setSceneRect(0,0,width, height);
}

void Strip::setHeight(double height)
{
    double width = this->sceneRect().width();

    this->setSceneRect(0,0,width, height);
}

double Strip::getWidth()
{
    return this->sceneRect().width();
}

double Strip::getHeight()
{
    return this->sceneRect().height();
}
