#include "Strip.h"

#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QPainter>

#include "nest/LB_NestConfig.h"
using namespace NestConfig;

Strip::Strip() : stripNb(0)
{    
}

double Strip::getSceneWidth() const
{
    return this->sceneRect().width();
}

double Strip::getSceneHeight() const
{
    return this->sceneRect().height();
}

double Strip::getStripWidth() const
{
    return STRIP_WIDTH;
}

double Strip::getStripHeight() const
{
    return STRIP_HEIGHT;
}

void Strip::Reset()
{
    this->clear();
    InitSize();
    for(int i=0;i<stripNb;++i) {
        stripUsed[i] = 0;
    }
    stripNb = 0;
}

void Strip::InitSize()
{
    this->setSceneRect(0,0,STRIP_WIDTH,STRIP_HEIGHT);
}

int Strip::GetUsedNumber() const
{
    return stripNb;
}

double Strip::GetUtilization(int index) const
{
    return stripUsed[index];
}

QImage Strip::DumpToImage()
{
    QImage result(sceneRect().size().toSize(),QImage::Format_ARGB32);
    QPainter aPainter(&result);
    aPainter.setRenderHints(aPainter.renderHints() | QPainter::Antialiasing);
    this->render(&aPainter);
    return result;
}

void Strip::AddOneStrip()
{
    stripNb++;
    setSceneRect(0,0,stripNb*STRIP_WIDTH,STRIP_HEIGHT);
    QGraphicsRectItem *rect = new QGraphicsRectItem(QRectF((stripNb-1)*STRIP_WIDTH,
                                                           0,
                                                           STRIP_WIDTH,
                                                           STRIP_HEIGHT));
    rect->setPen(QColor(Qt::red));
    addItem(rect);
}

void Strip::AddOneItem(LB_Polygon2D poly)
{
    // add the area to array
    if(ITEM_GAP != 0) {
        poly = poly.Shrinking(ITEM_GAP);
    }

    stripUsed[poly.ID()] += abs(poly.Area());

    // move the item to the correct strip
    poly.Translate(poly.ID()*STRIP_WIDTH,0);
    QPolygonF target = poly.ToPolygonF();

    // add the item
    QGraphicsPolygonItem *anItem = new QGraphicsPolygonItem(target);
    anItem->setPen(Qt::NoPen);
    anItem->setBrush(RandomColor());
    anItem->setFlags(QGraphicsItem::ItemIsMovable);
    addItem(anItem);
}
