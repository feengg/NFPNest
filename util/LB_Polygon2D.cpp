#include "LB_Polygon2D.h"
namespace Shape2D {

LB_Polygon2D::LB_Polygon2D(std::initializer_list<LB_Coord2D> list) : QVector<LB_Coord2D>(list)
{
    if(list.size() == 0)
        return;

    LB_Rect2D bounds = Bounds();
    x = bounds.X();
    y = bounds.Y();
    width = bounds.Width();
    height = bounds.Height();
}

double LB_Polygon2D::Area() const
{
    // https://zhuanlan.zhihu.com/p/110025234
    double area = 0;
    int i, j;
    for (i=0, j=size()-1; i<size(); j=i++){
        area += (at(j).X()+at(i).X()) * (at(j).Y()-at(i).Y());
    }
    return 0.5*area;
}

void LB_Polygon2D::Rotate(double angle)
{
    angle = angle * DEG2RAD;
    foreach(LB_Coord2D pnt,this){
        double x = pnt.X();
        double y = pnt.Y();
        pnt.RX() = x*cos(angle)-y*sin(angle);
        pnt.RY() = x*sin(angle)+y*cos(angle);
    }
    // reset bounding box
    LB_Rect2D bounds = Bounds();
    x = bounds.X();
    y = bounds.Y();
    width = bounds.Width();
    height = bounds.Height();
}

void LB_Polygon2D::Translate(double dx, double dy)
{
    foreach(LB_Coord2D pnt,this){
        pnt.RX() += dx;
        pnt.RY() += dy;
    }
    // reset bounding box
    LB_Rect2D bounds = Bounds();
    x = bounds.X();
    y = bounds.Y();
    width = bounds.Width();
    height = bounds.Height();
}

LB_Rect2D LB_Polygon2D::Bounds() const
{
    if(size() < 3){
        return INVALID_RECT;
    }

    double xmin = at(0).X();
    double xmax = at(0).X();
    double ymin = at(0).Y();
    double ymax = at(0).Y();

    for(int i=1; i<size(); i++){
        if(at(i).X() > xmax){
            xmax = at(i).X();
        }
        else if(at(i).X() < xmin){
            xmin = at(i).X();
        }

        if(at(i).Y() > ymax){
            ymax = at(i).Y();
        }
        else if(at(i).Y() < ymin){
            ymin = at(i).Y();
        }
    }

    return LB_Rect2D(xmin,
                     ymin,
                     xmax-xmin,
                     ymax-ymin);
}

void LB_Polygon2D::SetLocation(double px, double py)
{
    LB_Rect2D bnd = Bounds();
    double dx = px - bnd.X();
    double dy = py - bnd.Y();
    Translate(dx,dy);
}

void LB_Polygon2D::SetLocation(const LB_Coord2D &pnt)
{
    SetLocation(pnt.X(),pnt.Y());
}

void LB_Polygon2D::SetPosition(double px, double py, int index)
{
    if(index<0 || index>size()-1)
        return;

    double dx = px - at(index).X();
    double dy = py - at(index).Y();
    Translate(dx,dy);
}

void LB_Polygon2D::SetPosition(const LB_Coord2D &pnt, int index)
{
    SetPosition(pnt.X(),pnt.Y(),index);
}

bool LB_Polygon2D::IsConvex() const
{
    int size = this->size();

    bool frsign = LB_Coord2D::ZCrossProduct(at(0),at(1),at(2)) > 0;
    bool ret = true;
    for(int i=0;i<size;++i) {
        bool zc = LB_Coord2D::ZCrossProduct(at(i),
                                            at((i+1)%size),
                                            at((i+2)%size));
        ret &= frsign == (zc > 0);
    }

    return ret;
}

void LB_Polygon2D::SetAntiClockWise()
{
    if(IsAntiClockWise())
        return;

    std::reverse(this->begin(),this->end());
}

QPolygonF LB_Polygon2D::ToPolygonF() const
{
    QPolygonF shape;
    for(int i=0;i<size();++i) {
        shape.append(QPointF(at(i).X(),at(i).Y()));
    }
    return shape;
}

void LB_Polygon2D::FromPolygonF(const QPolygonF &aPoly)
{
    this->clear();
    foreach(QPointF pnt,aPoly) {
        this->append(LB_Coord2D(pnt.x(),pnt.y()));
    }
}

PointInPolygon LB_Polygon2D::ContainPoint(const LB_Coord2D &point) const
{
    // https://blog.csdn.net/hjh2005/article/details/9246967
    if(size() < 3){
        return PointInPolygon::INVALID;
    }

    bool inside = false;

    for (int i = 0, j = size() - 1; i < size(); j=i++) {
        double xi = at(i).X();
        double yi = at(i).Y();
        double xj = at(j).X();
        double yj = at(j).Y();

        if(FuzzyEqual(xi, point.X()) && FuzzyEqual(yi, point.Y())){
            return PointInPolygon::INVALID; // no result
        }

        if(LB_Coord2D::OnSegment(at(i), at(j), point)){
            return PointInPolygon::INVALID; // exactly on the segment
        }

        if(FuzzyEqual(xi, xj) && FuzzyEqual(yi, yj)){ // ignore very small lines
            continue;
        }

        bool intersect = ((yi > point.Y()) != (yj > point.Y())) && (point.X() < (xj - xi) * (point.Y() - yi) / (yj - yi) + xi);
        if (intersect) inside = !inside;
    }

    return inside ? PointInPolygon::INSIDE : PointInPolygon::OUTSIDE;
}

bool LB_Polygon2D::Intersect(const LB_Polygon2D &other) const
{
    for(int i=0; i<size()-1; i++){
        for(int j=0; j<other.size()-1; j++){
            LB_Coord2D a1 = at(i);
            LB_Coord2D a2 = at(i+1);
            LB_Coord2D b1 = other.at(i);
            LB_Coord2D b2 = other.at(i+1);

            int prevbindex = (j == 0) ? other.size()-1 : j-1;
            int prevaindex = (i == 0) ? size()-1 : i-1;
            int nextbindex = (j+1 == other.size()-1) ? 0 : j+2;
            int nextaindex = (i+1 == size()-1) ? 0 : i+2;

            // go even further back if we happen to hit on a loop end LB_Coord2D
            if(other[prevbindex] == other[j]){
                prevbindex = (prevbindex == 0) ? other.size()-1 : prevbindex-1;
            }

            if(at(prevaindex) == at(i)){
                prevaindex = (prevaindex == 0) ? size()-1 : prevaindex-1;
            }

            // go even further forward if we happen to hit on a loop end LB_Coord2D
            if(other[nextbindex] == other[j+1]){
                nextbindex = (nextbindex == other.size()-1) ? 0 : nextbindex+1;
            }

            if(at(nextaindex) == at(i+1)){
                nextaindex = (nextaindex == size()-1) ? 0 : nextaindex+1;
            }

            LB_Coord2D a0 = at(prevaindex);
            LB_Coord2D b0 = other[prevbindex];

            LB_Coord2D a3 = at(nextaindex);
            LB_Coord2D b3 = other[nextbindex];

            if(LB_Coord2D::OnSegment(a1,a2,b1) || (a1 == b1)){
                // if a LB_Coord2D is on a segment, it could intersect or it could not. Check via the neighboring LB_Coord2Ds
                PointInPolygon b0in = ContainPoint(b0);
                PointInPolygon b2in = ContainPoint(b2);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((b0in == PointInPolygon::INSIDE && b2in == PointInPolygon::OUTSIDE)
                        ||(b0in == PointInPolygon::OUTSIDE && b2in == PointInPolygon::INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            if(LB_Coord2D::OnSegment(a1,a2,b2) || (a2 == b2)){
                // if a LB_Coord2D is on a segment, it could intersect or it could not. Check via the neighboring LB_Coord2Ds
                PointInPolygon b1in = ContainPoint(b1);
                PointInPolygon b3in = ContainPoint(b3);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((b1in == PointInPolygon::INSIDE && b3in == PointInPolygon::OUTSIDE)
                        || (b1in == PointInPolygon::OUTSIDE && b3in == PointInPolygon::INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            if(LB_Coord2D::OnSegment(b1,b2,a1) || (a1 == b2)){
                // if a LB_Coord2D is on a segment, it could intersect or it could not. Check via the neighboring LB_Coord2Ds
                PointInPolygon a0in = other.ContainPoint(a0);
                PointInPolygon a2in = other.ContainPoint(a2);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((a0in == PointInPolygon::INSIDE && a2in == PointInPolygon::OUTSIDE)
                        || (a0in == PointInPolygon::OUTSIDE && a2in == PointInPolygon::INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            if(LB_Coord2D::OnSegment(b1,b2,a2) || (a2 == b1)){
                // if a LB_Coord2D is on a segment, it could intersect or it could not. Check via the neighboring LB_Coord2Ds
                PointInPolygon a1in = other.ContainPoint(a1);
                PointInPolygon a3in = other.ContainPoint(a3);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((a1in == PointInPolygon::INSIDE && a3in == PointInPolygon::OUTSIDE)
                        || (a1in == PointInPolygon::OUTSIDE && a3in == PointInPolygon::INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            LB_Coord2D p = LB_Coord2D::LineIntersect(b1, b2, a1, a2);

            if(p != INVALID_POINT){
                return true;
            }
        }
    }

    return false;
}

bool LB_Polygon2D::IsRectangle(double tolerance)
{
    LB_Rect2D bb = Bounds();

    for (int i = 0; i < size(); i++) {
        if (!FuzzyEqual(at(i).X(), bb.X(), tolerance)
                && !FuzzyEqual(at(i).X(), bb.X() + bb.Width(), tolerance)) {
            return false;
        }
        if (!FuzzyEqual(at(i).Y(), bb.Y(), tolerance)
                && !FuzzyEqual(at(i).Y(), bb.Y() + bb.Height(), tolerance)) {
            return false;
        }
    }

    return true;
}

}
