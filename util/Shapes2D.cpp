#include "Shapes2D.h"

double Polygon::Area() const
{
    double area = 0;
    int i, j;
    for (i=0, j=size()-1; i<size(); j=i++){
        area += (operator[](j).x+operator[](i).x) * (operator[](j).y-operator[](i).y);
    }
    return 0.5*area;
}

void Polygon::Rotate(double angle)
{
    angle = angle * DEG2RAD;
    for(int i=0; i<size(); i++){
        double x = operator[](i).x;
        double y = operator[](i).y;
        operator[](i).x = x*cos(angle)-y*sin(angle);
        operator[](i).y = x*sin(angle)+y*cos(angle);
    }
    // reset bounding box
    Rect bounds = Bounds();
    x = bounds.x;
    y = bounds.y;
    width = bounds.width;
    height = bounds.height;
}

void Polygon::Translate(double dx, double dy)
{
    for(int i=0; i<size(); i++){
        operator[](i).x += dx;
        operator[](i).y += dy;
    }
    Rect bounds = Bounds();
    x = bounds.x;
    y = bounds.y;
    width = bounds.width;
    height = bounds.height;
}

Rect Polygon::Bounds() const
{
    if(size() < 3){
        return INVALID_RECT;
    }

    double xmin = operator[](0).x;
    double xmax = operator[](0).x;
    double ymin = operator[](0).y;
    double ymax = operator[](0).y;

    for(int i=1; i<size(); i++){
        if(operator[](i).x > xmax){
            xmax = operator[](i).x;
        }
        else if(operator[](i).x < xmin){
            xmin = operator[](i).x;
        }

        if(operator[](i).y > ymax){
            ymax = operator[](i).y;
        }
        else if(operator[](i).y < ymin){
            ymin = operator[](i).y;
        }
    }

    return {
        xmin,
                ymin,
                xmax-xmin,
                ymax-ymin
    };
}

void Polygon::SetLocation(double px, double py)
{
    Rect bnd = Bounds();
    double dx = px - bnd.x;
    double dy = py - bnd.y;
    Translate(dx,dy);
}

void Polygon::SetLocation(const Point &pnt)
{
    SetLocation(pnt.x,pnt.y);
}

void Polygon::SetPosition(double px, double py, int index)
{
    if(index<0 || index>size()-1)
        return;

    double dx = px - operator[](index).x;
    double dy = py - operator[](index).y;
    Translate(dx,dy);
}

void Polygon::SetPosition(const Point &pnt, int index)
{
    SetPosition(pnt.x,pnt.y,index);
}

bool Polygon::IsConvex() const
{
    auto zcrossproduct = [](Point k, Point k1, Point k2) {
        auto dx1 = k1.x - k.x;
        auto dy1 = k1.y - k.y;
        auto dx2 = k2.x - k1.x;
        auto dy2 = k2.y - k1.y;
        return dx1*dy2 - dy1*dx2;
    };

    int size = this->size();

    bool frsign = zcrossproduct(operator[](0),operator[](1),operator[](2)) > 0;
            bool ret = true;
    for(int i=0;i<size;++i) {
        auto zc = zcrossproduct(operator[](i),
                operator[]((i+1)%size),
                operator[]((i+2)%size));
        ret &= frsign == (zc > 0);
    }

    return ret;
}

void Polygon::SetAntiClockWise()
{
    if(IsAntiClockWise())
        return;

    std::reverse(this->begin(),this->end());
}

QPolygonF Polygon::ToPolygonF() const
{
    QPolygonF shape;
    for(int i=0;i<size();++i) {
        shape.append(QPointF(operator[](i).x,operator[](i).y));
    }
    return shape;
}

void Polygon::FromPolygonF(const QPolygonF &aPoly)
{
    this->clear();
    foreach(QPointF pnt,aPoly) {
        this->append(Point(pnt.x(),pnt.y()));
    }
}
