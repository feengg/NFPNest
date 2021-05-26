#include "GeometryUtil.h"

namespace QGeometryUtil {
bool AlmostEqual(double a, double b, double tolerance) {
    return fabs(a - b) < tolerance;
}

bool WithinDistance(const Point& p1, const Point& p2, double distance){
    double dx = p1.x-p2.x;
    double dy = p1.y-p2.y;
    return ((dx*dx + dy*dy) < distance*distance);
}

Point NormalizeVec(const Point& v){
    if(AlmostEqual(v.x*v.x + v.y*v.y, 1)){
        return v; // given vector was already a unit vector
    }
    double len = sqrt(v.x*v.x + v.y*v.y);
    double inverse = 1/len;

    return { v.x*inverse,v.y*inverse };
}


bool OnSegment(const Point& A,const Point& B, const Point& p){

    // vertical line
    if(AlmostEqual(A.x, B.x) && AlmostEqual(p.x, A.x)){
        if(!AlmostEqual(p.y, B.y) && !AlmostEqual(p.y, A.y) && p.y < std::max(B.y, A.y) && p.y > std::min(B.y, A.y)){
            return true;
        }
        else{
            return false;
        }
    }

    // horizontal line
    if(AlmostEqual(A.y, B.y) && AlmostEqual(p.y, A.y)){
        if(!AlmostEqual(p.x, B.x) && !AlmostEqual(p.x, A.x) && p.x < std::max(B.x, A.x) && p.x > std::min(B.x, A.x)){
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
    if((AlmostEqual(p.x, A.x) && AlmostEqual(p.y, A.y)) || (AlmostEqual(p.x, B.x) && AlmostEqual(p.y, B.y))){
        return false;
    }

    double cross = (p.y - A.y) * (B.x - A.x) - (p.x - A.x) * (B.y - A.y);

    if(fabs(cross) > FLOAT_TOL){
        return false;
    }

    double dot = (p.x - A.x) * (B.x - A.x) + (p.y - A.y)*(B.y - A.y);



    if(dot < 0 || AlmostEqual(dot, 0)){
        return false;
    }

    double len2 = (B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y);

    if(dot > len2 || AlmostEqual(dot, len2)){
        return false;
    }

    return true;
}

Point LineIntersect(const Point& A,const Point& B,const Point& E,const Point& F, bool infinite){
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
        return INVALID_POINT;
    }

    // lines are colinear
    /*var crossABE = (E.y - A.y) * (B.x - A.x) - (E.x - A.x) * (B.y - A.y);
    var crossABF = (F.y - A.y) * (B.x - A.x) - (F.x - A.x) * (B.y - A.y);
    if(_almostEqual(crossABE,0) && _almostEqual(crossABF,0)){
        return null;
    }*/

    if(!infinite){
        // coincident points do not count as intersecting
        if (fabs(A.x-B.x) > FLOAT_TOL && (( A.x < B.x ) ? x < A.x || x > B.x : x > A.x || x < B.x )) return INVALID_POINT;
        if (fabs(A.y-B.y) > FLOAT_TOL && (( A.y < B.y ) ? y < A.y || y > B.y : y > A.y || y < B.y )) return INVALID_POINT;

        if (fabs(E.x-F.x) > FLOAT_TOL && (( E.x < F.x ) ? x < E.x || x > F.x : x > E.x || x < F.x )) return INVALID_POINT;
        if (fabs(E.y-F.y) > FLOAT_TOL && (( E.y < F.y ) ? y < E.y || y > F.y : y > E.y || y < F.y )) return INVALID_POINT;
    }

    return {x, y};
}

PointInPolygonResult InPolygon(const Point& point, const Polygon& polygon){
    if(polygon.size() < 3){
        return INVALID;
    }

    bool inside = false;
    double offsetx = polygon.offsetx;
    double offsety = polygon.offsety;

    for (int i = 0, j = polygon.size() - 1; i < polygon.size(); j=i++) {
        double xi = polygon[i].x + offsetx;
        double yi = polygon[i].y + offsety;
        double xj = polygon[j].x + offsetx;
        double yj = polygon[j].y + offsety;

        if(AlmostEqual(xi, point.x) && AlmostEqual(yi, point.y)){
            return INVALID; // no result
        }

        if(OnSegment({xi, yi}, {xj, yj}, point)){
            return INVALID; // exactly on the segment
        }

        if(AlmostEqual(xi, xj) && AlmostEqual(yi, yj)){ // ignore very small lines
            continue;
        }

        bool intersect = ((yi > point.y) != (yj > point.y)) && (point.x < (xj - xi) * (point.y - yi) / (yj - yi) + xi);
        if (intersect) inside = !inside;
    }

    return inside ? INSIDE : OUTSIDE;
}

bool Intersect(const Polygon& A, const Polygon& B){
    double Aoffsetx = A.offsetx;
    double Aoffsety = A.offsety;

    double Boffsetx = B.offsetx;
    double Boffsety = B.offsety;


    for(int i=0; i<A.size()-1; i++){
        for(int j=0; j<B.size()-1; j++){
            Point a1 = {A[i].x+Aoffsetx, A[i].y+Aoffsety};
            Point a2 = {A[i+1].x+Aoffsetx, A[i+1].y+Aoffsety};
            Point b1 = {B[j].x+Boffsetx, B[j].y+Boffsety};
            Point b2 = {B[j+1].x+Boffsetx, B[j+1].y+Boffsety};

            int prevbindex = (j == 0) ? B.size()-1 : j-1;
            int prevaindex = (i == 0) ? A.size()-1 : i-1;
            int nextbindex = (j+1 == B.size()-1) ? 0 : j+2;
            int nextaindex = (i+1 == A.size()-1) ? 0 : i+2;

            // go even further back if we happen to hit on a loop end point
            if(B[prevbindex] == B[j] || (AlmostEqual(B[prevbindex].x, B[j].x) && AlmostEqual(B[prevbindex].y, B[j].y))){
                prevbindex = (prevbindex == 0) ? B.size()-1 : prevbindex-1;
            }

            if(A[prevaindex] == A[i] || (AlmostEqual(A[prevaindex].x, A[i].x) && AlmostEqual(A[prevaindex].y, A[i].y))){
                prevaindex = (prevaindex == 0) ? A.size()-1 : prevaindex-1;
            }

            // go even further forward if we happen to hit on a loop end point
            if(B[nextbindex] == B[j+1] || (AlmostEqual(B[nextbindex].x, B[j+1].x) && AlmostEqual(B[nextbindex].y, B[j+1].y))){
                nextbindex = (nextbindex == B.size()-1) ? 0 : nextbindex+1;
            }

            if(A[nextaindex] == A[i+1] || (AlmostEqual(A[nextaindex].x, A[i+1].x) && AlmostEqual(A[nextaindex].y, A[i+1].y))){
                nextaindex = (nextaindex == A.size()-1) ? 0 : nextaindex+1;
            }

            Point a0 = {A[prevaindex].x + Aoffsetx, A[prevaindex].y + Aoffsety};
            Point b0 = {B[prevbindex].x + Boffsetx, B[prevbindex].y + Boffsety};

            Point a3 = {A[nextaindex].x + Aoffsetx, A[nextaindex].y + Aoffsety};
            Point b3 = {B[nextbindex].x + Boffsetx, B[nextbindex].y + Boffsety};

            if(OnSegment(a1,a2,b1) || (AlmostEqual(a1.x, b1.x) && AlmostEqual(a1.y, b1.y))){
                // if a point is on a segment, it could intersect or it could not. Check via the neighboring points
                PointInPolygonResult b0in = InPolygon(b0, A);
                PointInPolygonResult b2in = InPolygon(b2, A);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((b0in == INSIDE && b2in == OUTSIDE) || (b0in == OUTSIDE && b2in == INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            if(OnSegment(a1,a2,b2) || (AlmostEqual(a2.x, b2.x) && AlmostEqual(a2.y, b2.y))){
                // if a point is on a segment, it could intersect or it could not. Check via the neighboring points
                PointInPolygonResult b1in = InPolygon(b1, A);
                PointInPolygonResult b3in = InPolygon(b3, A);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((b1in == INSIDE && b3in == OUTSIDE) || (b1in == OUTSIDE && b3in == INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            if(OnSegment(b1,b2,a1) || (AlmostEqual(a1.x, b2.x) && AlmostEqual(a1.y, b2.y))){
                // if a point is on a segment, it could intersect or it could not. Check via the neighboring points
                PointInPolygonResult a0in = InPolygon(a0, B);
                PointInPolygonResult a2in = InPolygon(a2, B);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((a0in == INSIDE && a2in == OUTSIDE) || (a0in == OUTSIDE && a2in == INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            if(OnSegment(b1,b2,a2) || (AlmostEqual(a2.x, b1.x) && AlmostEqual(a2.y, b1.y))){
                // if a point is on a segment, it could intersect or it could not. Check via the neighboring points
                PointInPolygonResult a1in = InPolygon(a1, B);
                PointInPolygonResult a3in = InPolygon(a3, B);
                //AMIR: TESTME is this comparison correct in terms of handling INVALID?
                if((a1in == INSIDE && a3in == OUTSIDE) || (a1in == OUTSIDE && a3in == INSIDE)){
                    return true;
                }
                else{
                    continue;
                }
            }

            Point p = LineIntersect(b1, b2, a1, a2);

            if(p != INVALID_POINT){
                return true;
            }
        }
    }

    return false;
}


std::vector<Point> PolygonEdge(const Polygon& polygon, Point normal){
    if(polygon.size() < 3){
        return {};
    }

    normal = NormalizeVec(normal);

    Point direction = {
        -normal.y,
        normal.x
    };

    // find the max and min points, they will be the endpoints of our edge
    double min = DIM_MAX;
    double max = DIM_MIN;

    std::vector<double> dotproduct;

    for(int i=0; i<polygon.size(); i++){
        double dot = polygon[i].x*direction.x + polygon[i].y*direction.y;
        dotproduct.push_back(dot);
        if(min == DIM_MAX || dot < min){
            min = dot;
        }
        if(max == DIM_MIN || dot > max){
            max = dot;
        }
    }

    // there may be multiple vertices with min/max values. In which case we choose the one that is normal-most (eg. left most)
    int indexmin = 0;
    int indexmax = 0;

    double normalmin = DIM_MAX;
    double normalmax = DIM_MIN;

    for(int i=0; i<polygon.size(); i++){
        if(AlmostEqual(dotproduct[i] , min)){
            double dot = polygon[i].x*normal.x + polygon[i].y*normal.y;
            if(normalmin == DIM_MAX || dot > normalmin){
                normalmin = dot;
                indexmin = i;
            }
        }
        else if(AlmostEqual(dotproduct[i] , max)){
            double dot = polygon[i].x*normal.x + polygon[i].y*normal.y;
            if(normalmax == DIM_MIN || dot > normalmax){
                normalmax = dot;
                indexmax = i;
            }
        }
    }

    // now we have two edges bound by min and max points, figure out which edge faces our direction vector

    int indexleft = indexmin-1;
    int indexright = indexmin+1;

    if(indexleft < 0){
        indexleft = polygon.size()-1;
    }
    if(indexright >= polygon.size()){
        indexright = 0;
    }

    const Point& minvertex = polygon[indexmin];
    const Point& left = polygon[indexleft];
    const Point& right = polygon[indexright];

    Point leftvector = {
        left.x - minvertex.x,
        left.y - minvertex.y
    };

    Point rightvector = {
        right.x - minvertex.x,
        right.y - minvertex.y
    };

    double dotleft = leftvector.x*direction.x + leftvector.y*direction.y;
    double dotright = rightvector.x*direction.x + rightvector.y*direction.y;

    // -1 = left, 1 = right
    int8_t scandirection = -1;

    if(AlmostEqual(dotleft, 0)){
        scandirection = 1;
    }
    else if(AlmostEqual(dotright, 0)){
        scandirection = -1;
    }
    else{
        double normaldotleft;
        double normaldotright;

        if(AlmostEqual(dotleft, dotright)){
            // the points line up exactly along the normal vector
            normaldotleft = leftvector.x*normal.x + leftvector.y*normal.y;
            normaldotright = rightvector.x*normal.x + rightvector.y*normal.y;
        }
        else if(dotleft < dotright){
            // normalize right vertex so normal projection can be directly compared
            normaldotleft = leftvector.x*normal.x + leftvector.y*normal.y;
            normaldotright = (rightvector.x*normal.x + rightvector.y*normal.y)*(dotleft/dotright);
        }
        else{
            // normalize left vertex so normal projection can be directly compared
            normaldotleft = leftvector.x*normal.x + leftvector.y*normal.y * (dotright/dotleft);
            normaldotright = rightvector.x*normal.x + rightvector.y*normal.y;
        }

        if(normaldotleft > normaldotright){
            scandirection = -1;
        }
        else{
            // technically they could be equal, (ie. the segments bound by left and right points are incident)
            // in which case we'll have to climb up the chain until lines are no longer incident
            // for now we'll just not handle it and assume people aren't giving us garbage input..
            scandirection = 1;
        }
    }

    // connect all points between indexmin and indexmax along the scan direction
    std::vector<Point> edge;
    int count = 0;
    int i=indexmin;
    while(count < polygon.size()){
        if(i >= polygon.size()){
            i=0;
        }
        else if(i < 0){
            i=polygon.size()-1;
        }

        edge.push_back(polygon[i]);

        if(i == indexmax){
            break;
        }
        i += scandirection;
        count++;
    }

    return edge;
}

double pointLineDistance(const Point& p, const Point& s1, const Point& s2, Point normal, bool s1inclusive, bool s2inclusive){

    normal = NormalizeVec(normal);

    Point dir = {
        normal.y,
        -normal.x
    };

    double pdot = p.x*dir.x + p.y*dir.y;
    double s1dot = s1.x*dir.x + s1.y*dir.y;
    double s2dot = s2.x*dir.x + s2.y*dir.y;

    double pdotnorm = p.x*normal.x + p.y*normal.y;
    double s1dotnorm = s1.x*normal.x + s1.y*normal.y;
    double s2dotnorm = s2.x*normal.x + s2.y*normal.y;


    // point is exactly along the edge in the normal direction
    if(AlmostEqual(pdot, s1dot) && AlmostEqual(pdot, s2dot)){
        // point lies on an endpoint
        if(AlmostEqual(pdotnorm, s1dotnorm) ){
            return DIM_MAX;
        }

        if(AlmostEqual(pdotnorm, s2dotnorm)){
            return DIM_MAX;
        }

        // point is outside both endpoints
        if (pdotnorm>s1dotnorm && pdotnorm>s2dotnorm){
            return std::min(pdotnorm - s1dotnorm, pdotnorm - s2dotnorm);
        }
        if (pdotnorm<s1dotnorm && pdotnorm<s2dotnorm){
            return -std::min(s1dotnorm-pdotnorm, s2dotnorm-pdotnorm);
        }

        // point lies between endpoints
        double diff1 = pdotnorm - s1dotnorm;
        double diff2 = pdotnorm - s2dotnorm;
        if(diff1 > 0){
            return diff1;
        }
        else{
            return diff2;
        }
    }
    // point
    else if(AlmostEqual(pdot, s1dot)){
        if(s1inclusive){
            return pdotnorm-s1dotnorm;
        }
        else{
            return DIM_MAX;
        }
    }
    else if(AlmostEqual(pdot, s2dot)){
        if(s2inclusive){
            return pdotnorm-s2dotnorm;
        }
        else{
            return DIM_MAX;
        }
    }
    else if ((pdot<s1dot && pdot<s2dot) || (pdot>s1dot && pdot>s2dot)){
        return DIM_MAX; // point doesn't collide with segment
    }

    return (pdotnorm - s1dotnorm + (s1dotnorm - s2dotnorm)*(s1dot - pdot)/(s1dot - s2dot));
}

double pointDistance(const Point& p, const Point& s1, const Point& s2,
                    Point normal, bool infinite) {
    normal = NormalizeVec(normal);

    Point dir = { normal.y, -normal.x };

    double pdot = p.x * dir.x + p.y * dir.y;
    double s1dot = s1.x * dir.x + s1.y * dir.y;
    double s2dot = s2.x * dir.x + s2.y * dir.y;

    double pdotnorm = p.x * normal.x + p.y * normal.y;
    double s1dotnorm = s1.x * normal.x + s1.y * normal.y;
    double s2dotnorm = s2.x * normal.x + s2.y * normal.y;

    if (!infinite) {
        if (((pdot < s1dot || AlmostEqual(pdot, s1dot))
             && (pdot < s2dot || AlmostEqual(pdot, s2dot)))
                || ((pdot > s1dot || AlmostEqual(pdot, s1dot))
                    && (pdot > s2dot || AlmostEqual(pdot, s2dot)))) {
            return DIM_MAX; // dot doesn't collide with segment, or lies directly on the vertex
        }
        if ((AlmostEqual(pdot, s1dot) && AlmostEqual(pdot, s2dot))
                && (pdotnorm > s1dotnorm && pdotnorm > s2dotnorm)) {
            return std::min(pdotnorm - s1dotnorm, pdotnorm - s2dotnorm);
        }
        if ((AlmostEqual(pdot, s1dot) && AlmostEqual(pdot, s2dot))
                && (pdotnorm < s1dotnorm && pdotnorm < s2dotnorm)) {
            return -std::min(s1dotnorm - pdotnorm, s2dotnorm - pdotnorm);
        }
    }

    return -(pdotnorm - s1dotnorm
             + (s1dotnorm - s2dotnorm) * (s1dot - pdot) / (s1dot - s2dot));
}

double segmentDistance(const Point& A, const Point& B, const Point& E,
                      const Point& F, const Point& direction) {
    Point normal = { direction.y, -direction.x };

    Point reverse = { -direction.x, -direction.y };

    double dotA = A.x * normal.x + A.y * normal.y;
    double dotB = B.x * normal.x + B.y * normal.y;
    double dotE = E.x * normal.x + E.y * normal.y;
    double dotF = F.x * normal.x + F.y * normal.y;

    double crossA = A.x * direction.x + A.y * direction.y;
    double crossB = B.x * direction.x + B.y * direction.y;
    double crossE = E.x * direction.x + E.y * direction.y;
    double crossF = F.x * direction.x + F.y * direction.y;

    double ABmin = std::min(dotA, dotB);
    double ABmax = std::max(dotA, dotB);

    double EFmax = std::max(dotE, dotF);
    double EFmin = std::min(dotE, dotF);

    // segments that will merely touch at one point
    if (AlmostEqual(ABmax, EFmin, FLOAT_TOL)
            || AlmostEqual(ABmin, EFmax, FLOAT_TOL)) {
        return DIM_MAX;
    }
    // segments miss eachother completely
    if (ABmax < EFmin || ABmin > EFmax) {
        return DIM_MAX;
    }

    double overlap;

    if ((ABmax > EFmax && ABmin < EFmin) || (EFmax > ABmax && EFmin < ABmin)) {
        overlap = 1;
    } else {
        double minMax = std::min(ABmax, EFmax);
        double maxMin = std::max(ABmin, EFmin);

        double maxMax = std::max(ABmax, EFmax);
        double minMin = std::min(ABmin, EFmin);

        overlap = (minMax - maxMin) / (maxMax - minMin);
    }

    double crossABE = (E.y - A.y) * (B.x - A.x) - (E.x - A.x) * (B.y - A.y);
    double crossABF = (F.y - A.y) * (B.x - A.x) - (F.x - A.x) * (B.y - A.y);

    // lines are colinear
    if (AlmostEqual(crossABE, 0) && AlmostEqual(crossABF, 0)) {

        Point ABnorm = { B.y - A.y, A.x - B.x };
        Point EFnorm = { F.y - E.y, E.x - F.x };

        double ABnormlength = sqrt(ABnorm.x * ABnorm.x + ABnorm.y * ABnorm.y);
        ABnorm.x /= ABnormlength;
        ABnorm.y /= ABnormlength;

        double EFnormlength = sqrt(EFnorm.x * EFnorm.x + EFnorm.y * EFnorm.y);
        EFnorm.x /= EFnormlength;
        EFnorm.y /= EFnormlength;

        // segment normals must point in opposite directions
        if (fabs(ABnorm.y * EFnorm.x - ABnorm.x * EFnorm.y) < FLOAT_TOL
                && ABnorm.y * EFnorm.y + ABnorm.x * EFnorm.x < 0) {
            // normal of AB segment must point in same direction as given direction vector
            double normdot = ABnorm.y * direction.y + ABnorm.x * direction.x;
            // the segments merely slide along eachother
            if (AlmostEqual(normdot, 0, FLOAT_TOL)) {
                return DIM_MAX;
            }
            if (normdot < 0) {
                return 0;
            }
        }
        return DIM_MAX;
    }

    std::vector<double> distances;

    // coincident points
    if (AlmostEqual(dotA, dotE)) {
        distances.push_back(crossA - crossE);
    } else if (AlmostEqual(dotA, dotF)) {
        distances.push_back(crossA - crossF);
    } else if (dotA > EFmin && dotA < EFmax) {
        double d = pointDistance(A, E, F, reverse);
        if (d != DIM_MAX && AlmostEqual(d, 0)) { //  A currently touches EF, but AB is moving away from EF
            double dB = pointDistance(B, E, F, reverse, true);
            if (dB < 0 || AlmostEqual(dB * overlap, 0)) {
                d = DIM_MAX;
            }
        }
        if (d != DIM_MAX) {
            distances.push_back(d);
        }
    }

    if (AlmostEqual(dotB, dotE)) {
        distances.push_back(crossB - crossE);
    } else if (AlmostEqual(dotB, dotF)) {
        distances.push_back(crossB - crossF);
    } else if (dotB > EFmin && dotB < EFmax) {
        double d = pointDistance(B, E, F, reverse);

        if (d != DIM_MAX && AlmostEqual(d, 0)) { // crossA>crossB A currently touches EF, but AB is moving away from EF
            double dA = pointDistance(A, E, F, reverse, true);
            if (dA < 0 || AlmostEqual(dA * overlap, 0)) {
                d = DIM_MAX;
            }
        }
        if (d != DIM_MAX) {
            distances.push_back(d);
        }
    }

    if (dotE > ABmin && dotE < ABmax) {
        double d = pointDistance(E, A, B, direction);
        if (d != DIM_MAX && AlmostEqual(d, 0)) { // crossF<crossE A currently touches EF, but AB is moving away from EF
            double dF = pointDistance(F, A, B, direction, true);
            if (dF < 0 || AlmostEqual(dF * overlap, 0)) {
                d = DIM_MAX;
            }
        }
        if (d != DIM_MAX) {
            distances.push_back(d);
        }
    }

    if (dotF > ABmin && dotF < ABmax) {
        double d = pointDistance(F, A, B, direction);
        if (d != DIM_MAX && AlmostEqual(d, 0)) { // && crossE<crossF A currently touches EF, but AB is moving away from EF
            double dE = pointDistance(E, A, B, direction, true);
            if (dE < 0 || AlmostEqual(dE * overlap, 0)) {
                d = DIM_MAX;
            }
        }
        if (d != DIM_MAX) {
            distances.push_back(d);
        }
    }

    if (distances.size() == 0) {
        return DIM_MAX;
    }

    return *std::min_element(distances.begin(), distances.end());
}

double polygonSlideDistance(Polygon A, Polygon B, const Point& direction,
                           bool ignoreNegative) {

    Point A1, A2, B1, B2;
    double Aoffsetx, Aoffsety, Boffsetx, Boffsety;

    Aoffsetx = A.offsetx;
    Aoffsety = A.offsety;

    Boffsetx = B.offsetx;
    Boffsety = B.offsety;

    // close the loop for polygons
    if (A[0] != A[A.size() - 1]) {
        A.push_back(A.front());
    }

    if (B[0] != B[B.size() - 1]) {
        B.push_back(B.front());
    }

    Polygon edgeA = A;
    Polygon edgeB = B;

    double distance = DIM_MAX;
    double d;

    Point dir = NormalizeVec(direction);

    for (int i = 0; i < edgeB.size() - 1; i++) {
        for (int j = 0; j < edgeA.size() - 1; j++) {
            A1 = {edgeA[j].x + Aoffsetx, edgeA[j].y + Aoffsety};
            A2 = {edgeA[j+1].x + Aoffsetx, edgeA[j+1].y + Aoffsety};
            B1 = {edgeB[i].x + Boffsetx, edgeB[i].y + Boffsety};
            B2 = {edgeB[i+1].x + Boffsetx, edgeB[i+1].y + Boffsety};

            if((AlmostEqual(A1.x, A2.x) && AlmostEqual(A1.y, A2.y)) || (AlmostEqual(B1.x, B2.x) && AlmostEqual(B1.y, B2.y))) {
                continue; // ignore extremely small lines
            }

            d = segmentDistance(A1, A2, B1, B2, dir);

            if(d != DIM_MAX && (distance == DIM_MAX || d < distance)) {
                if(!ignoreNegative || d > 0 || AlmostEqual(d, 0.0)) {
                    distance = d;
                }
            }
        }
    }
    return distance;
}

double polygonProjectionDistance(Polygon A, Polygon B, const Point& direction) {
    double Boffsetx = B.offsetx;
    double Boffsety = B.offsety;

    double Aoffsetx = A.offsetx;
    double Aoffsety = A.offsety;

    // close the loop for polygons
    if (A[0] != A[A.size() - 1]) {
        A.push_back(A[0]);
    }

    if (B[0] != B[B.size() - 1]) {
        B.push_back(B[0]);
    }

    Polygon edgeA = A;
    Polygon edgeB = B;

    double distance = DIM_MAX;
    Point p, s1, s2;
    double d;

    for (int i = 0; i < edgeB.size(); i++) {
        // the shortest/most negative projection of B onto A
        double minprojection = DIM_MAX;
        Point minp = INVALID_POINT;
        for (int j = 0; j < edgeA.size() - 1; j++) {
            p = {edgeB[i].x + Boffsetx, edgeB[i].y + Boffsety};
            s1 = {edgeA[j].x + Aoffsetx, edgeA[j].y + Aoffsety};
            s2 = {edgeA[j+1].x + Aoffsetx, edgeA[j+1].y + Aoffsety};

            if(fabs((s2.y-s1.y) * direction.x - (s2.x-s1.x) * direction.y) < FLOAT_TOL) {
                continue;
            }

            // project point, ignore edge boundaries
            d = pointDistance(p, s1, s2, direction);

            if(d != DIM_MAX && (minprojection == DIM_MAX || d < minprojection)) {
                minprojection = d;
                minp = p;
            }
        }
        if (minprojection != DIM_MAX
                && (distance == DIM_MAX || minprojection > distance)) {
            distance = minprojection;
        }
    }

    return distance;
}

// returns true if point already exists in the given nfp
bool inNfp(const Point& p, const QVector<Polygon>& nfp) {
    if (nfp.empty()) {
        return false;
    }

    for (int i = 0; i < nfp.size(); i++) {
        for (int j = 0; j < nfp[i].size(); j++) {
            if (AlmostEqual(p.x, nfp[i][j].x) && AlmostEqual(p.y, nfp[i][j].y)) {
                return true;
            }
        }
    }

    return false;
}
Point searchStartPoint(Polygon A, Polygon B, bool inside,
                       const QVector<Polygon>& NFP) {

    // close the loop for polygons
    if (A[0] != A[A.size() - 1]) {
        A.push_back(A[0]);
    }

    if (B[0] != B[B.size() - 1]) {
        B.push_back(B[0]);
    }

    for (int i = 0; i < A.size() - 1; i++) {
        if (!A[i].marked) {
            A[i].marked = true;
            for (int j = 0; j < B.size(); j++) {
                B.offsetx = A[i].x - B[j].x;
                B.offsety = A[i].y - B[j].y;

                PointInPolygonResult Binside = INVALID;
                for (int k = 0; k < B.size(); k++) {
                    PointInPolygonResult inpoly = InPolygon( { B[k].x + B.offsetx,
                                                                    B[k].y + B.offsety }, A);
                    if (inpoly != INVALID) {
                        Binside = inpoly;
                        break;
                    }
                }

                if (Binside == INVALID) { // A and B are the same
                    return INVALID_POINT;
                }

                Point startPoint = { B.offsetx, B.offsety };
                if (((Binside && inside) || (!Binside && !inside)) && !Intersect(A, B)
                        && !inNfp(startPoint, NFP)) {
                    return startPoint;
                }

                // slide B along vector
                double vx = A[i + 1].x - A[i].x;
                double vy = A[i + 1].y - A[i].y;

                double d1 = polygonProjectionDistance(A, B, {vx, vy });
                double d2 = polygonProjectionDistance(B, A, {-vx, -vy });

                double d = DIM_MAX;

                // todo: clean this up
                if (d1 == DIM_MAX && d2 == DIM_MAX) {
                    // nothin
                } else if (d1 == DIM_MAX) {
                    d = d2;
                } else if (d2 == DIM_MAX) {
                    d = d1;
                } else {
                    d = std::min(d1, d2);
                }

                // only slide until no longer negative
                // todo: clean this up
                if (d == DIM_MAX && !AlmostEqual(d, 0) && d > 0) {

                } else {
                    continue;
                }

                double vd2 = vx * vx + vy * vy;

                if (d * d < vd2 && !AlmostEqual(d * d, vd2)) {
                    double vd = sqrt(vx * vx + vy * vy);
                    vx *= d / vd;
                    vy *= d / vd;
                }

                B.offsetx += vx;
                B.offsety += vy;

                for (int k = 0; k < B.size(); k++) {
                    PointInPolygonResult inpoly = InPolygon( { B[k].x + B.offsetx,
                                                                    B[k].y + B.offsety }, A);
                    if (inpoly == INVALID) {
                        Binside = inpoly;
                        break;
                    }
                }
                startPoint = {B.offsetx, B.offsety};
                if (((Binside && inside) || (!Binside && !inside)) && !Intersect(A, B)
                        && !inNfp(startPoint, NFP)) {
                    return startPoint;
                }
            }
        }
    }

    return INVALID_POINT;
}

bool isRectangle(const Polygon& poly, double tolerance) {
    Rect bb = poly.Bounds();

    for (int i = 0; i < poly.size(); i++) {
        if (!AlmostEqual(poly[i].x, bb.x, tolerance)
                && !AlmostEqual(poly[i].x, bb.x + bb.width, tolerance)) {
            return false;
        }
        if (!AlmostEqual(poly[i].y, bb.y, tolerance)
                && !AlmostEqual(poly[i].y, bb.y + bb.height, tolerance)) {
            return false;
        }
    }

    return true;
}

// returns an interior NFP for the special case where A is a rectangle
QVector<Polygon> noFitPolygonRectangle(const Polygon& A, const Polygon& B) {
    double minAx = A[0].x;
    double minAy = A[0].y;
    double maxAx = A[0].x;
    double maxAy = A[0].y;

    for (int i = 1; i < A.size(); i++) {
        if (A[i].x < minAx) {
            minAx = A[i].x;
        }
        if (A[i].y < minAy) {
            minAy = A[i].y;
        }
        if (A[i].x > maxAx) {
            maxAx = A[i].x;
        }
        if (A[i].y > maxAy) {
            maxAy = A[i].y;
        }
    }

    double minBx = B[0].x;
    double minBy = B[0].y;
    double maxBx = B[0].x;
    double maxBy = B[0].y;

    for (int i = 1; i < B.size(); i++) {
        if (B[i].x < minBx) {
            minBx = B[i].x;
        }
        if (B[i].y < minBy) {
            minBy = B[i].y;
        }
        if (B[i].x > maxBx) {
            maxBx = B[i].x;
        }
        if (B[i].y > maxBy) {
            maxBy = B[i].y;
        }
    }

    if (maxBx - minBx > maxAx - minAx) {
        return {};
    }
    if (maxBy - minBy > maxAy - minAy) {
        return {};
    }

    return { {
            {	minAx-minBx+B[0].x, minAy-minBy+B[0].y},
            {	maxAx-maxBx+B[0].x, minAy-minBy+B[0].y},
            {	maxAx-maxBx+B[0].x, maxAy-maxBy+B[0].y},
            {	minAx-minBx+B[0].x, maxAy-maxBy+B[0].y}
        }};
}

// given a static polygon A and a movable polygon B, compute a no fit polygon by orbiting B about A
// if the inside flag is set, B is orbited inside of A rather than outside
// if the searchEdges flag is set, all edges of A are explored for NFPs - multiple
QVector<Polygon> noFitPolygon(Polygon A, Polygon B, bool inside, bool searchEdges){
    if(A.size() < 3 || B.size() < 3){
        return {};
    }

    A.offsetx = 0;
    A.offsety = 0;

    int i, j;

    double minA = A[0].y;
    int minAindex = 0;

    double maxB = B[0].y;
    int maxBindex = 0;

    for(i=1; i<A.size(); i++){
        A[i].marked = false;
        if(A[i].y < minA){
            minA = A[i].y;
            minAindex = i;
        }
    }

    for(i=1; i<B.size(); i++){
        B[i].marked = false;
        if(B[i].y > maxB){
            maxB = B[i].y;
            maxBindex = i;
        }
    }

    Point startpoint;
    if(!inside){
        // shift B such that the bottom-most point of B is at the top-most point of A. This guarantees an initial placement with no intersections
        startpoint = {
            A[minAindex].x-B[maxBindex].x,
            A[minAindex].y-B[maxBindex].y
        };
    }
    else{
        // no reliable heuristic for inside
        startpoint = searchStartPoint(A,B,true);
    }

    QVector<Polygon> NFPlist;

    struct EdgeDescriptor {
        int8_t type;
        int A;
        int B;
    };

    while(startpoint != INVALID_POINT){
        B.offsetx = startpoint.x;
        B.offsety = startpoint.y;


        // maintain a list of touching points/edges
        QVector<EdgeDescriptor> touching;

        Point prevvector = INVALID_POINT; // keep track of previous vector
        Polygon NFP;
        NFP.push_back({B[0].x+B.offsetx,B[0].y+B.offsety});

        double referencex = B[0].x+B.offsetx;
        double referencey = B[0].y+B.offsety;
        double startx = referencex;
        double starty = referencey;
        int counter = 0;

        while(counter < 10*(A.size() + B.size())){ // sanity check, prevent infinite loop
            touching = {};
            // find touching vertices/edges
            for(i=0; i<A.size(); i++){
                int nexti = (i==A.size()-1) ? 0 : i+1;
                for(j=0; j<B.size(); j++){
                    int nextj = (j==B.size()-1) ? 0 : j+1;
                    if(AlmostEqual(A[i].x, B[j].x+B.offsetx) && AlmostEqual(A[i].y, B[j].y+B.offsety)){
                        touching.push_back({ 0, i, j });
                    }
                    else if(OnSegment(A[i],A[nexti],{B[j].x+B.offsetx, B[j].y + B.offsety})){
                        touching.push_back({	1, nexti, j });
                    }
                    else if(OnSegment({B[j].x+B.offsetx, B[j].y + B.offsety},{B[nextj].x+B.offsetx, B[nextj].y + B.offsety},A[i])){
                        touching.push_back({	2, i, nextj });
                    }
                }
            }


            struct TransVector {
                double x;
                double y;
                Point start;
                Point end;

                bool operator==(const TransVector& other) const {
                    return this->x == other.x && this->y == other.y && this->start == other.start && this->end == other.end;
                }

                bool operator!=(const TransVector& other) const {
                    return !this->operator==(other);
                }
            };

            const TransVector INVALID_TRANSVECTOR = {DIM_MAX, DIM_MAX, INVALID_POINT, INVALID_POINT};
            // generate translation vectors from touching vertices/edges
            QVector<TransVector> vectors;
            for(i=0; i<touching.size(); i++){
                Point vertexA = A[touching[i].A];
                vertexA.marked = true;

                // adjacent A vertices
                int prevAindex = touching[i].A-1;
                int nextAindex = touching[i].A+1;

                prevAindex = (prevAindex < 0) ? A.size()-1 : prevAindex; // loop
                nextAindex = (nextAindex >= A.size()) ? 0 : nextAindex; // loop

                Point prevA = A[prevAindex];
                Point nextA = A[nextAindex];

                // adjacent B vertices
                Point vertexB = B[touching[i].B];

                int prevBindex = touching[i].B-1;
                int nextBindex = touching[i].B+1;

                prevBindex = (prevBindex < 0) ? B.size()-1 : prevBindex; // loop
                nextBindex = (nextBindex >= B.size()) ? 0 : nextBindex; // loop

                Point prevB = B[prevBindex];
                Point nextB = B[nextBindex];

                if(touching[i].type == 0){
                    TransVector vA1 = {
                        prevA.x-vertexA.x,
                        prevA.y-vertexA.y,
                        vertexA,
                        prevA
                    };

                    TransVector vA2 = {
                        nextA.x-vertexA.x,
                        nextA.y-vertexA.y,
                        vertexA,
                        nextA
                    };

                    // B vectors need to be inverted
                    TransVector vB1 = {
                        vertexB.x-prevB.x,
                        vertexB.y-prevB.y,
                        prevB,
                        vertexB
                    };

                    TransVector vB2 = {
                        vertexB.x-nextB.x,
                        vertexB.y-nextB.y,
                        nextB,
                        vertexB
                    };

                    vectors.push_back(vA1);
                    vectors.push_back(vA2);
                    vectors.push_back(vB1);
                    vectors.push_back(vB2);
                }
                else if(touching[i].type == 1){
                    vectors.push_back({
                                          vertexA.x-(vertexB.x+B.offsetx),
                                          vertexA.y-(vertexB.y+B.offsety),
                                          prevA,
                                          vertexA
                                      });

                    vectors.push_back({
                                          prevA.x-(vertexB.x+B.offsetx),
                                          prevA.y-(vertexB.y+B.offsety),
                                          vertexA,
                                          prevA
                                      });
                }
                else if(touching[i].type == 2){
                    vectors.push_back({
                                          vertexA.x-(vertexB.x+B.offsetx),
                                          vertexA.y-(vertexB.y+B.offsety),
                                          prevB,
                                          vertexB
                                      });

                    vectors.push_back({
                                          vertexA.x-(prevB.x+B.offsetx),
                                          vertexA.y-(prevB.y+B.offsety),
                                          vertexB,
                                          prevB
                                      });
                }
            }

            // todo: there should be a faster way to reject vectors that will cause immediate intersection. For now just check them all

            TransVector translate = INVALID_TRANSVECTOR;
            double maxd = 0;

            for(i=0; i<vectors.size(); i++){
                if(vectors[i].x == 0 && vectors[i].y == 0){
                    continue;
                }

                // if this vector points us back to where we came from, ignore it.
                // ie cross product = 0, dot product < 0
                if(prevvector != INVALID_POINT && vectors[i].y * prevvector.y + vectors[i].x * prevvector.x < 0){
                    // compare magnitude with unit vectors
                    double vectorlength = sqrt(vectors[i].x*vectors[i].x+vectors[i].y*vectors[i].y);
                    Point unitv = {vectors[i].x/vectorlength, vectors[i].y/vectorlength};

                    double prevlength = sqrt(prevvector.x*prevvector.x+prevvector.y*prevvector.y);
                    Point prevunit = {prevvector.x/prevlength, prevvector.y/prevlength};

                    // we need to scale down to unit vectors to normalize vector length. Could also just do a tan here
                    if(fabs(unitv.y * prevunit.x - unitv.x * prevunit.y) < 0.0001){
                        continue;
                    }
                }

                Point pv = {vectors[i].x, vectors[i].y};
                double d = polygonSlideDistance(A, B, pv, true);
                double vecd2 = vectors[i].x*vectors[i].x + vectors[i].y*vectors[i].y;

                if(d == DIM_MAX || d*d > vecd2){
                    double vecd = sqrt(vectors[i].x*vectors[i].x + vectors[i].y*vectors[i].y);
                    d = vecd;
                }

                if(d != DIM_MAX && d > maxd){
                    maxd = d;
                    translate = vectors[i];
                }
            }


            if(translate == INVALID_TRANSVECTOR || AlmostEqual(maxd, 0)){
                // didn't close the loop, something went wrong here
                NFP = {};
                break;
            }

            translate.start.marked = true;
            translate.end.marked = true;

            prevvector = Point(translate.x, translate.y);

            // trim
            double vlength2 = translate.x*translate.x + translate.y*translate.y;
            if(maxd*maxd < vlength2 && !AlmostEqual(maxd*maxd, vlength2)){
                double scale = sqrt((maxd*maxd)/vlength2);
                translate.x *= scale;
                translate.y *= scale;
            }

            referencex += translate.x;
            referencey += translate.y;

            if(AlmostEqual(referencex, startx) && AlmostEqual(referencey, starty)){
                // we've made a full loop
                break;
            }

            // if A and B start on a touching horizontal line, the end point may not be the start point
            bool looped = false;
            if(!NFP.empty()){
                for(i=0; i<NFP.size()-1; i++){
                    if(AlmostEqual(referencex, NFP[i].x) && AlmostEqual(referencey, NFP[i].y)){
                        looped = true;
                    }
                }
            }

            if(looped){
                // we've made a full loop
                break;
            }

            NFP.push_back({
                              referencex,
                              referencey
                          });

            B.offsetx += translate.x;
            B.offsety += translate.y;

            counter++;
        }

        if(!NFP.empty()){
            NFPlist.push_back(NFP);
        }

        if(!searchEdges){
            // only get outer NFP or first inner NFP
            break;
        }

        startpoint = searchStartPoint(A,B,inside,NFPlist);
    }

    return NFPlist;
}

// given two polygons that touch at at least one point, but do not intersect. Return the outer perimeter of both polygons as a single continuous polygon
// A and B must have the same winding direction
Polygon polygonHull(Polygon A, Polygon B) {
    if (A.size() < 3 || B.size() < 3) {
        return {};
    }

    int i, j;

    double Aoffsetx = A.offsetx;
    double Aoffsety = A.offsety;
    double Boffsetx = B.offsetx;
    double Boffsety = B.offsety;

    // start at an extreme point that is guaranteed to be on the final polygon
    double miny = A[0].y;
    //FIXME: AMIR use pointers
    Polygon startPolygon = A;
    int startIndex = 0;

    for (i = 0; i < A.size(); i++) {
        if (A[i].y + Aoffsety < miny) {
            miny = A[i].y + Aoffsety;
            startPolygon = A;
            startIndex = i;
        }
    }

    for (i = 0; i < B.size(); i++) {
        if (B[i].y + Boffsety < miny) {
            miny = B[i].y + Boffsety;
            startPolygon = B;
            startIndex = i;
        }
    }

    // for simplicity we'll define polygon A as the starting polygon
    if (startPolygon == B) {
        B = A;
        A = startPolygon;
        Aoffsetx = A.offsetx || 0;
        Aoffsety = A.offsety || 0;
        Boffsetx = B.offsetx || 0;
        Boffsety = B.offsety || 0;
    }

    QVector<Point> C;
    int current = startIndex;
    int intercept1 = std::numeric_limits<int>::max();
    int intercept2 = std::numeric_limits<int>::max();

    // scan forward from the starting point
    for (i = 0; i < A.size() + 1; i++) {
        current = (current == A.size()) ? 0 : current;
        int next = (current == A.size() - 1) ? 0 : current + 1;
        bool touching = false;
        for (j = 0; j < B.size(); j++) {
            int nextj = (j == B.size() - 1) ? 0 : j + 1;
            if (AlmostEqual(A[current].x + Aoffsetx, B[j].x + Boffsetx)
                    && AlmostEqual(A[current].y + Aoffsety, B[j].y + Boffsety)) {
                C.push_back( { A[current].x + Aoffsetx, A[current].y + Aoffsety });
                intercept1 = j;
                touching = true;
                break;
            } else if (OnSegment(
            { A[current].x + Aoffsetx, A[current].y + Aoffsety },
            { A[next].x + Aoffsetx, A[next].y + Aoffsety },
            { B[j].x + Boffsetx, B[j].y + Boffsety })) {
                C.push_back( { A[current].x + Aoffsetx, A[current].y + Aoffsety });
                C.push_back( { B[j].x + Boffsetx, B[j].y + Boffsety });
                intercept1 = j;
                touching = true;
                break;
            } else if (OnSegment( { B[j].x + Boffsetx, B[j].y + Boffsety },
            {B[nextj].x + Boffsetx, B[nextj].y + Boffsety },
            { A[current].x + Aoffsetx, A[current].y + Aoffsety })) {
                C.push_back( { A[current].x + Aoffsetx, A[current].y + Aoffsety });
                C.push_back( { B[nextj].x + Boffsetx, B[nextj].y + Boffsety });
                intercept1 = j;
                touching = true;
                break;
            }
        }

        if (touching) {
            break;
        }

        C.push_back( { A[current].x + Aoffsetx, A[current].y + Aoffsety });

        current++;
    }

    // scan backward from the starting point
    current = startIndex - 1;
    for (i = 0; i < A.size() + 1; i++) {
        current = (current < 0) ? A.size() - 1 : current;
        int next = (current == 0) ? A.size() - 1 : current - 1;
        bool touching = false;
        for (j = 0; j < B.size(); j++) {
            int nextj = (j == B.size() - 1) ? 0 : j + 1;
            if (AlmostEqual(A[current].x + Aoffsetx, B[j].x + Boffsetx)
                    && AlmostEqual(A[current].y, B[j].y + Boffsety)) {
                C.push_front( { A[current].x + Aoffsetx, A[current].y + Aoffsety });
                intercept2 = j;
                touching = true;
                break;
            } else if (OnSegment(
            { A[current].x + Aoffsetx, A[current].y + Aoffsety },
            { A[next].x + Aoffsetx, A[next].y + Aoffsety },
            { B[j].x + Boffsetx, B[j].y + Boffsety })) {
                C.push_front( { A[current].x + Aoffsetx, A[current].y + Aoffsety });
                C.push_front( { B[j].x + Boffsetx, B[j].y + Boffsety });
                intercept2 = j;
                touching = true;
                break;
            } else if (OnSegment(
            { B[j].x + Boffsetx, B[j].y + Boffsety },
            { B[nextj].x + Boffsetx, B[nextj].y + Boffsety },
            { A[current].x + Aoffsetx, A[current].y + Aoffsety })) {
                C.push_front( { A[current].x + Aoffsetx, A[current].y + Aoffsety });
                intercept2 = j;
                touching = true;
                break;
            }
        }

        if (touching) {
            break;
        }

        C.push_front( { A[current].x + Aoffsetx, A[current].y + Aoffsety });

        current--;
    }

    if (intercept1 == std::numeric_limits<int>::max()
            || intercept2 == std::numeric_limits<int>::max()) {
        // polygons not touching?
        return {};
    }

    // the relevant points on B now lie between intercept1 and intercept2
    current = intercept1 + 1;
    for (i = 0; i < B.size(); i++) {
        current = (current == B.size()) ? 0 : current;
        C.push_back( { B[current].x + Boffsetx, B[current].y + Boffsety });

        if (current == intercept2) {
            break;
        }

        current++;
    }

    // dedupe
    //FIXME: AMIR we need to copy the list because list supports push_front and vector supports access by index
    Polygon PC;
    std::copy(std::begin(C), std::end(C), std::back_inserter(PC));

    for (i = 0; i < PC.size(); i++) {
        int next = (i == PC.size() - 1) ? 0 : i + 1;
        if (AlmostEqual(PC[i].x, PC[next].x)
                && AlmostEqual(PC[i].y, PC[next].y)) {
            PC.erase(PC.begin() + i);
            i--;
        }
    }

    return PC;
}

Polygon polygonUnion(Polygon A, Polygon B)
{
    if (A.size() < 3 || B.size() < 3) {
        return {};
    }

    int i, j;

    // start at an extreme point that is guaranteed to be on the final polygon
    double miny = A[0].y;
    //FIXME: AMIR use pointers
    Polygon startPolygon = A;
    int startIndex = 0;

    for (i = 0; i < A.size(); i++) {
        if (A[i].y < miny) {
            miny = A[i].y;
            startPolygon = A;
            startIndex = i;
        }
    }

    for (i = 0; i < B.size(); i++) {
        if (B[i].y < miny) {
            miny = B[i].y;
            startPolygon = B;
            startIndex = i;
        }
    }

    // for simplicity we'll define polygon A as the starting polygon
    if (startPolygon == B) {
        B = A;
        A = startPolygon;
    }

    Polygon C;
    int current = startIndex;
    int intercept1 = -1;
    int intercept2 = -1;

    // scan forward from the starting point
    for (i = 0; i < A.size() + 1; i++) {
        current = (current == A.size()) ? 0 : current;
        int next = (current == A.size() - 1) ? 0 : current + 1;
        bool touching = false;
        for (j = 0; j < B.size(); j++) {
            int nextj = (j == B.size() - 1) ? 0 : j + 1;
            if (A[current] == B[j]) {
                C.push_back(A[current]);
                intercept1 = j;
                touching = true;
                break;
            } else if (OnSegment(A[current],A[next],B[j])) {
                C.push_back(A[current]);
                C.push_back(B[j]);
                intercept1 = j;
                touching = true;
                break;
            } else if (OnSegment(B[j],B[nextj],A[current])) {
                C.push_back(A[current]);
                C.push_back(B[nextj]);
                intercept1 = j;
                touching = true;
                break;
            }
        }

        if (touching) {
            break;
        }

        C.push_back(A[current]);

        current++;
    }

    // scan backward from the starting point
    current = startIndex - 1;
    for (i = 0; i < A.size() + 1; i++) {
        current = (current < 0) ? A.size() - 1 : current;
        int next = (current == 0) ? A.size() - 1 : current - 1;
        bool touching = false;
        for (j = 0; j < B.size(); j++) {
            int nextj = (j == B.size() - 1) ? 0 : j + 1;
            if (A[current] == B[j]) {
                C.push_front(A[current]);
                intercept2 = j;
                touching = true;
                break;
            } else if (OnSegment(A[current],A[next],B[j])) {
                C.push_front(A[current]);
                C.push_front(B[j]);
                intercept2 = j;
                touching = true;
                break;
            } else if (OnSegment(B[j],B[nextj],A[current])) {
                C.push_front(A[current]);
                intercept2 = j;
                touching = true;
                break;
            }
        }

        if (touching) {
            break;
        }

        C.push_front(A[current]);

        current--;
    }

    if (intercept1 == -1 || intercept2 == -1) {
        // polygons not touching?
        return {};
    }

    // the relevant points on B now lie between intercept1 and intercept2
    current = intercept1 + 1;
    for (i = 0; i < B.size(); i++) {
        current = (current == B.size()) ? 0 : current;
        C.push_back(B[current]);

        if (current == intercept2) {
            break;
        }

        current++;
    }

    for (i = 0; i < C.size(); i++) {
        int next = (i == C.size() - 1) ? 0 : i + 1;
        if (C[i] ==  C[next]) {
            C.erase(C.begin() + i);
            i--;
        }
    }

    for (i = 0; i < C.size(); i++) {
        int prei = (i ==  0)? C.size() - 1 : i - 1;
        for (j = C.size()-1; j>i; j--) {
            int prej = (j ==  0)? C.size() - 1 : j - 1;
            if (C[i] ==  C[j]) {
                C[i].x = 0.9*C[i].x + 0.1*C[prei].x;
                C[i].y = 0.9*C[i].y + 0.1*C[prei].y;
                C[j].x = 0.9*C[j].x + 0.1*C[prej].x;
                C[j].y = 0.9*C[j].y + 0.1*C[prej].y;
            }
        }
    }

    return C;
}

} //GeometryUtil
