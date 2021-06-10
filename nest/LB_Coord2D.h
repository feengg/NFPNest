#ifndef LB_COORD2D_H
#define LB_COORD2D_H

#include "LB_BaseUtil.h"
using namespace BaseUtil;

namespace Shape2D {

class LB_Coord2D
{
public:
    LB_Coord2D(){}
    LB_Coord2D(double x, double y): x(x), y(y) {}

    bool operator==(const LB_Coord2D& other) const {
        return FuzzyEqual(this->x, other.x) && FuzzyEqual(this->y, other.y);
    }

    bool operator!=(const LB_Coord2D& other) const {
        return !this->operator==(other);
    }

    LB_Coord2D operator+(const LB_Coord2D& other) const {
        return LB_Coord2D{x+other.x,y+other.y};
    }
    LB_Coord2D operator-(const LB_Coord2D& other) const {
        return LB_Coord2D{x-other.x,y-other.y};
    }
    LB_Coord2D operator*(double val) const {
        return LB_Coord2D{x*val,y*val};
    }

    double Dot(const LB_Coord2D& other) const {
        return x*other.x + y*other.y;
    }
    double Cross(const LB_Coord2D& other) const {
        return x*other.y - y*other.x;
    }

    double X() const {
        return x;
    }
    double Y() const {
        return y;
    }

    double& RX() {
        return x;
    }
    double& RY() {
        return y;
    }

    bool Marked() const {
        return marked;
    }
    void setMarked(bool ret) {
        marked = ret;
    }    

    void Normalize() {
        if(FuzzyEqual(x*x + y*y, 1)){
            return; // vector was already a unit vector
        }
        double len = sqrt(x*x + y*y);
        double inverse = 1/len;
        x *= inverse;
        y *= inverse;
    }
    LB_Coord2D Normalized() const {
        if(FuzzyEqual(x*x + y*y, 1)){
            return *this; // vector was already a unit vector
        }
        double len = sqrt(x*x + y*y);
        double inverse = 1/len;
        return this->operator*(inverse);
    }

    static double ZCrossProduct(const LB_Coord2D &k, const LB_Coord2D &k1, const LB_Coord2D &k2) {
        double dx1 = k1.X() - k.X();
        double dy1 = k1.Y() - k.Y();
        double dx2 = k2.X() - k1.X();
        double dy2 = k2.Y() - k1.Y();
        return dx1*dy2 - dy1*dx2;
    }

    static bool OnSegment(const LB_Coord2D &A,const LB_Coord2D &B, const LB_Coord2D &p) {
        // vertical line
        if(FuzzyEqual(A.x, B.x) && FuzzyEqual(p.x, A.x)){
            if(!FuzzyEqual(p.y, B.y) && !FuzzyEqual(p.y, A.y) && p.y < std::fmax(B.y, A.y) && p.y > std::fmin(B.y, A.y)){
                return true;
            }
            else{
                return false;
            }
        }

        // horizontal line
        if(FuzzyEqual(A.y, B.y) && FuzzyEqual(p.y, A.y)){
            if(!FuzzyEqual(p.x, B.x) && !FuzzyEqual(p.x, A.x) && p.x < std::fmax(B.x, A.x) && p.x > std::fmin(B.x, A.x)){
                return true;
            }
            else{
                return false;
            }
        }

        //range check
        if((p.x < A.x && p.x < B.x) || (p.x > A.x && p.x > B.x) || (p.y < A.y && p.y < B.y) || (p.y > A.y && p.y > B.y)){
            return false;
        }

        // exclude end points
        if((FuzzyEqual(p.x, A.x) && FuzzyEqual(p.y, A.y)) || (FuzzyEqual(p.x, B.x) && FuzzyEqual(p.y, B.y))){
            return false;
        }

        double cross = (p.y - A.y) * (B.x - A.x) - (p.x - A.x) * (B.y - A.y);

        if(fabs(cross) > FLOAT_TOL){
            return false;
        }

        double dot = (p.x - A.x) * (B.x - A.x) + (p.y - A.y)*(B.y - A.y);

        if(dot < 0 || FuzzyEqual(dot, 0)){
            return false;
        }

        double len2 = (B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y);

        if(dot > len2 || FuzzyEqual(dot, len2)){
            return false;
        }

        return true;
    }

    // returns the intersection of AB and EF
    // or null if there are no intersections or other numerical error
    // if the infinite flag is set, AE and EF describe infinite lines without endpoints, they are finite line segments otherwise
    static LB_Coord2D LineIntersect(const LB_Coord2D& A,const LB_Coord2D& B,const LB_Coord2D& E,const LB_Coord2D& F, bool infinite = false) {
        double a1, a2, b1, b2, c1, c2, x, y;

        a1= B.y-A.y;
        b1= A.x-B.x;
        c1= B.x*A.y - A.x*B.y;
        a2= F.y-E.y;
        b2= E.x-F.x;
        c2= F.x*E.y - E.x*F.y;

        double denom=a1*b2 - a2*b1;

        x = (b1*c2 - b2*c1)/denom;
        y = (a2*c1 - a1*c2)/denom;

        if(!std::isfinite(x) || !std::isfinite(y)){
            return LB_Coord2D(DIM_MAX,DIM_MAX);
        }

        if(!infinite){
            // coincident points do not count as intersecting
            if (fabs(A.x-B.x) > FLOAT_TOL && (( A.x < B.x ) ? x < A.x || x > B.x : x > A.x || x < B.x )) return LB_Coord2D(DIM_MAX,DIM_MAX);
            if (fabs(A.y-B.y) > FLOAT_TOL && (( A.y < B.y ) ? y < A.y || y > B.y : y > A.y || y < B.y )) return LB_Coord2D(DIM_MAX,DIM_MAX);

            if (fabs(E.x-F.x) > FLOAT_TOL && (( E.x < F.x ) ? x < E.x || x > F.x : x > E.x || x < F.x )) return LB_Coord2D(DIM_MAX,DIM_MAX);
            if (fabs(E.y-F.y) > FLOAT_TOL && (( E.y < F.y ) ? y < E.y || y > F.y : y > E.y || y < F.y )) return LB_Coord2D(DIM_MAX,DIM_MAX);
        }

        return {x, y};
    }

private:
    double x = 0;
    double y = 0;
    bool marked = false;
};

const LB_Coord2D INVALID_POINT(DIM_MAX,DIM_MAX);

}

#endif // LB_COORD2D_H
