#ifndef LB_RECT2D_H
#define LB_RECT2D_H

#include "LB_BaseUtil.h"
#include "LB_Coord2D.h"

namespace Shape2D {

class LB_Rect2D
{
public:
    LB_Rect2D(){}
    LB_Rect2D(double x, double y, double width, double height): x(x), y(y), width(width), height(height) {}
    LB_Rect2D(const LB_Coord2D &aCoord, double width, double height): x(aCoord.X()), y(aCoord.Y()), width(width), height(height) {}

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

private:
    double x = 0;
    double y = 0;
    double width = 0;
    double height = 0;
};

const LB_Rect2D INVALID_RECT(DIM_MAX,DIM_MAX,DIM_MAX,DIM_MAX);

}

#endif // LB_RECT2D_H
