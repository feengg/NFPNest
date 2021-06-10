#ifndef LB_POLYGON2D_H
#define LB_POLYGON2D_H

#include <QVector>
#include <QPolygonF>

#include "LB_Rect2D.h"

namespace Shape2D {

enum PointInPolygon {
    INSIDE,
    OUTSIDE,
    INVALID
};

class LB_Polygon2D : public QVector<LB_Coord2D>
{
public:
    LB_Polygon2D(){}
    LB_Polygon2D(std::initializer_list<LB_Coord2D> list);

    double X() const {
        return x;
    }
    double Y() const {
        return y;
    }
    double Width() const {
        return width;
    }
    double Height() const {
        return height;
    }
    int ID() const {
        return stripID;
    }
    void SetID(int val) {
        stripID = val;
    }
    QString ToString() const;

    double Area() const;

    void Rotate(double angle);
    void Translate(double dx, double dy);
    LB_Rect2D Bounds() const;
    int RotateToMinBndRect();

    void SetLocation(double px, double py);
    void SetLocation(const LB_Coord2D &pnt);
    void SetPosition(double px, double py, int index);
    void SetPosition(const LB_Coord2D &pnt, int index);

    bool IsConvex() const;
    bool IsAntiClockWise() const {
        return this->Area()<0;
    }
    void SetAntiClockWise();

    QPolygonF ToPolygonF() const;
    void FromPolygonF(const QPolygonF &aPoly);

    PointInPolygon ContainPoint(const LB_Coord2D &point) const;

    bool Intersect(const LB_Polygon2D &other) const;

    bool IsRectangle(double tolerance = FLOAT_TOL);

    // given two polygons that touch at at least one point, but do not intersect. Return the outer perimeter of both polygons as a single continuous polygon
    // A and B must have the same winding direction
    LB_Polygon2D United(const LB_Polygon2D &other) const;

    LB_Polygon2D Shrinking(double offset) const;

private:    
    double x = 0;
    double y = 0;
    double width = 0;
    double height = 0;
    int stripID = -1;
};

}

#endif // LB_POLYGON2D_H
