#pragma once

#include <QVector>
#include <QPolygonF>
#include "Constants.h"

struct Point {
    Point() {};
    Point(double x, double y): x(x), y(y) {}
    double x = 0;
    double y = 0;
    bool marked = false;

    bool operator==(const Point& other) const {
        return abs(this->x - other.x)<FLOAT_TOL && abs(this->y - other.y)<FLOAT_TOL;
    }

    bool operator!=(const Point& other) const {
        return !this->operator==(other);
    }
};

struct Rect {
    Rect(double x, double y, double width, double height): x(x), y(y), width(width), height(height) {}
    double x = 0;
    double y = 0;
    double width = 0;
    double height = 0;
};

class Polygon : public QVector<Point> {
public:
    Polygon() {}
    Polygon(std::initializer_list<Point> list) : QVector<Point>(list) {
        if(list.size() == 0)
            return;

        Rect bounds = Bounds();
        x = bounds.x;
        y = bounds.y;
        width = bounds.width;
        height = bounds.height;
    }
    double offsetx = 0;
    double offsety = 0;
    double x = 0;
    double y = 0;
    double width = 0;
    double height = 0;
    int stripID = -1;

    double Area() const;
    void Rotate(double angle);
    void Translate(double dx, double dy);
    Rect Bounds() const;
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
};

const Point INVALID_POINT(DIM_MAX,DIM_MAX);
const Rect INVALID_RECT(DIM_MAX,DIM_MAX,DIM_MAX,DIM_MAX);
