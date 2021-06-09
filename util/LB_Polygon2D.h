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

class Polygon : public QVector<Point>
{
public:
    Polygon(){}
    Polygon(std::initializer_list<Point> list);

    double offsetx = 0;
    double offsety = 0;

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

    double Area() const;

    void Rotate(double angle);
    void Translate(double dx, double dy);
    LB_Rect2D Bounds() const;

    void SetLocation(double px, double py);
    void SetLocation(const Point &pnt);
    void SetPosition(double px, double py, int index);
    void SetPosition(const Point &pnt, int index);

    bool IsConvex() const;
    bool IsAntiClockWise() const {
        return this->Area()<0;
    }
    void SetAntiClockWise();

    QPolygonF ToPolygonF() const;
    void FromPolygonF(const QPolygonF &aPoly);

    PointInPolygon ContainPoint(const Point &point) const;

    bool Intersect(const Polygon &other) const;

    bool IsRectangle(double tolerance = FLOAT_TOL);

    Polygon United(const Polygon &other) const;

private:    
    double x = 0;
    double y = 0;
    double width = 0;
    double height = 0;
    int stripID = -1;
};

}

#endif // LB_POLYGON2D_H
