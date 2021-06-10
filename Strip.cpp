#include "Strip.h"

#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>

Strip::Strip(double width, double height) : stripWidth(width), stripHeight(height), stripNb(0)
{
    this->setSceneRect(0,0,width,height);
}

void Strip::setSceneWidth(double width)
{
    double height = this->sceneRect().height();
    this->setSceneRect(0,0,width, height);
}

void Strip::setSceneHeight(double height)
{
    double width = this->sceneRect().width();
    this->setSceneRect(0,0,width, height);
}

double Strip::getSceneWidth() const
{
    return this->sceneRect().width();
}

double Strip::getSceneHeight() const
{
    return this->sceneRect().height();
}

void Strip::setStripWidth(double val)
{
    stripWidth = val;
}

void Strip::setStripHeight(double val)
{
    stripHeight = val;
}

double Strip::getStripWidth() const
{
    return stripWidth;
}

double Strip::getStripHeight() const
{
    return stripHeight;
}

void Strip::Reset()
{
    this->clear();
    this->setSceneWidth(stripWidth);
    for(int i=0;i<stripNb;++i) {
        stripUsed[i] = 0;
    }
    stripNb = 0;
}

int Strip::GetUsedNumber() const
{
    return stripNb;
}

double Strip::GetUtilization(int index) const
{
    return stripUsed[index];
}

void Strip::AddOneStrip()
{
    stripNb++;
    setSceneWidth(stripNb*stripWidth);
    QGraphicsRectItem *rect = new QGraphicsRectItem(QRectF((stripNb-1)*stripWidth,
                                                           0,
                                                           stripWidth,
                                                           stripHeight));
    rect->setPen(QColor(Qt::red));
    addItem(rect);
}

void Strip::AddOneItem(LB_Polygon2D poly)
{
    // add the area to array
    stripUsed[poly.ID()] += abs(poly.Area());

    // move the item to the correct strip
    poly.Translate(poly.ID()*stripWidth,0);
    QPolygonF target = poly.ToPolygonF();

    // add the item
    QGraphicsPolygonItem *anItem = new QGraphicsPolygonItem(target);
    anItem->setPen(QColor(Qt::gray));
    anItem->setBrush(randomColor());
    anItem->setFlags(QGraphicsItem::ItemIsMovable);
    addItem(anItem);
}
