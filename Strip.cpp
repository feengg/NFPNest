#include "Strip.h"

#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QDebug>

#include "nest/LB_NestConfig.h"
using namespace NestConfig;

const double & stripWid = LB_NestConfig::STRIP_WIDTH;
const double & stripHei = LB_NestConfig::STRIP_HEIGHT;
const double & pieceGap = LB_NestConfig::ITEM_GAP;

Strip::Strip() : stripNb(0)
{
    InitSize();
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
    return stripWid;
}

double Strip::getStripHeight() const
{
    return stripHei;
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
    this->setSceneRect(0,0,stripWid,stripHei);
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
    setSceneRect(0,0,stripNb*stripWid,stripHei);
    QGraphicsRectItem *rect = new QGraphicsRectItem(QRectF((stripNb-1)*stripWid,
                                                           0,
                                                           stripWid,
                                                           stripHei));
    rect->setPen(QColor(Qt::red));
    addItem(rect);
}

void Strip::AddOneItem(LB_Polygon2D poly)
{
    // add the area to array
    if(pieceGap != 0) {
        poly = poly.Shrinking(pieceGap);
    }

    stripUsed[poly.ID()] += abs(poly.Area());

    // move the item to the correct strip
    poly.Translate(poly.ID()*stripWid,0);
    QPolygonF target = poly.ToPolygonF();

    // add the item
    QGraphicsPolygonItem *anItem = new QGraphicsPolygonItem(target);
    anItem->setPen(Qt::NoPen);
    anItem->setBrush(RandomColor());
    anItem->setFlags(QGraphicsItem::ItemIsMovable);
    addItem(anItem);
}
