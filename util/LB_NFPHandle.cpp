#include "LB_NFPHandle.h"

namespace LB_NFP {

double pointDistance(const LB_Coord2D &p, const LB_Coord2D &s1, const LB_Coord2D &s2, LB_Coord2D normal, bool infinite)
{
    normal.Normalize();

    LB_Coord2D dir(normal.Y(), -normal.X());

    double pdot = p.X() * dir.X() + p.Y() * dir.Y();
    double s1dot = s1.X() * dir.X() + s1.Y() * dir.Y();
    double s2dot = s2.X() * dir.X() + s2.Y() * dir.Y();

    double pdotnorm = p.X() * normal.X() + p.Y() * normal.Y();
    double s1dotnorm = s1.X() * normal.X() + s1.Y() * normal.Y();
    double s2dotnorm = s2.X() * normal.X() + s2.Y() * normal.Y();

    if (!infinite) {
        if (((pdot < s1dot || FuzzyEqual(pdot, s1dot))
             && (pdot < s2dot || FuzzyEqual(pdot, s2dot)))
                || ((pdot > s1dot || FuzzyEqual(pdot, s1dot))
                    && (pdot > s2dot || FuzzyEqual(pdot, s2dot)))) {
            return DIM_MAX; // dot doesn't collide with segment, or lies directly on the vertex
        }
        if ((FuzzyEqual(pdot, s1dot) && FuzzyEqual(pdot, s2dot))
                && (pdotnorm > s1dotnorm && pdotnorm > s2dotnorm)) {
            return std::min(pdotnorm - s1dotnorm, pdotnorm - s2dotnorm);
        }
        if ((FuzzyEqual(pdot, s1dot) && FuzzyEqual(pdot, s2dot))
                && (pdotnorm < s1dotnorm && pdotnorm < s2dotnorm)) {
            return -std::min(s1dotnorm - pdotnorm, s2dotnorm - pdotnorm);
        }
    }

    return -(pdotnorm - s1dotnorm
             + (s1dotnorm - s2dotnorm) * (s1dot - pdot) / (s1dot - s2dot));
}

double segmentDistance(const LB_Coord2D &A, const LB_Coord2D &B, const LB_Coord2D &E, const LB_Coord2D &F, const LB_Coord2D &direction)
{
    LB_Coord2D normal = { direction.Y(), -direction.X() };

    LB_Coord2D reverse = { -direction.X(), -direction.Y() };

    double dotA = A.X() * normal.X() + A.Y() * normal.Y();
    double dotB = B.X() * normal.X() + B.Y() * normal.Y();
    double dotE = E.X() * normal.X() + E.Y() * normal.Y();
    double dotF = F.X() * normal.X() + F.Y() * normal.Y();

    double crossA = A.X() * direction.X() + A.Y() * direction.Y();
    double crossB = B.X() * direction.X() + B.Y() * direction.Y();
    double crossE = E.X() * direction.X() + E.Y() * direction.Y();
    double crossF = F.X() * direction.X() + F.Y() * direction.Y();

    double ABmin = std::min(dotA, dotB);
    double ABmax = std::max(dotA, dotB);

    double EFmax = std::max(dotE, dotF);
    double EFmin = std::min(dotE, dotF);

    // segments that will merely touch at one point
    if (FuzzyEqual(ABmax, EFmin, FLOAT_TOL)
            || FuzzyEqual(ABmin, EFmax, FLOAT_TOL)) {
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

    double crossABE = (E.Y() - A.Y()) * (B.X() - A.X()) - (E.X() - A.X()) * (B.Y() - A.Y());
    double crossABF = (F.Y() - A.Y()) * (B.X() - A.X()) - (F.X() - A.X()) * (B.Y() - A.Y());

    // lines are colinear
    if (FuzzyEqual(crossABE, 0) && FuzzyEqual(crossABF, 0)) {

        LB_Coord2D ABnorm = { B.Y() - A.Y(), A.X() - B.X() };
        LB_Coord2D EFnorm = { F.Y() - E.Y(), E.X() - F.X() };

        double ABnormlength = sqrt(ABnorm.X() * ABnorm.X() + ABnorm.Y() * ABnorm.Y());
        ABnorm.RX() /= ABnormlength;
        ABnorm.RY() /= ABnormlength;

        double EFnormlength = sqrt(EFnorm.X() * EFnorm.X() + EFnorm.Y() * EFnorm.Y());
        EFnorm.RX() /= EFnormlength;
        EFnorm.RY() /= EFnormlength;

        // segment normals must point in opposite directions
        if (fabs(ABnorm.Y() * EFnorm.X() - ABnorm.X() * EFnorm.Y()) < FLOAT_TOL
                && ABnorm.Y() * EFnorm.Y() + ABnorm.X() * EFnorm.X() < 0) {
            // normal of AB segment must point in same direction as given direction vector
            double normdot = ABnorm.Y() * direction.Y() + ABnorm.X() * direction.X();
            // the segments merely slide along eachother
            if (FuzzyEqual(normdot, 0, FLOAT_TOL)) {
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
    if (FuzzyEqual(dotA, dotE)) {
        distances.push_back(crossA - crossE);
    } else if (FuzzyEqual(dotA, dotF)) {
        distances.push_back(crossA - crossF);
    } else if (dotA > EFmin && dotA < EFmax) {
        double d = pointDistance(A, E, F, reverse);
        if (d != DIM_MAX && FuzzyEqual(d, 0)) { //  A currently touches EF, but AB is moving away from EF
            double dB = pointDistance(B, E, F, reverse, true);
            if (dB < 0 || FuzzyEqual(dB * overlap, 0)) {
                d = DIM_MAX;
            }
        }
        if (d != DIM_MAX) {
            distances.push_back(d);
        }
    }

    if (FuzzyEqual(dotB, dotE)) {
        distances.push_back(crossB - crossE);
    } else if (FuzzyEqual(dotB, dotF)) {
        distances.push_back(crossB - crossF);
    } else if (dotB > EFmin && dotB < EFmax) {
        double d = pointDistance(B, E, F, reverse);

        if (d != DIM_MAX && FuzzyEqual(d, 0)) { // crossA>crossB A currently touches EF, but AB is moving away from EF
            double dA = pointDistance(A, E, F, reverse, true);
            if (dA < 0 || FuzzyEqual(dA * overlap, 0)) {
                d = DIM_MAX;
            }
        }
        if (d != DIM_MAX) {
            distances.push_back(d);
        }
    }

    if (dotE > ABmin && dotE < ABmax) {
        double d = pointDistance(E, A, B, direction);
        if (d != DIM_MAX && FuzzyEqual(d, 0)) { // crossF<crossE A currently touches EF, but AB is moving away from EF
            double dF = pointDistance(F, A, B, direction, true);
            if (dF < 0 || FuzzyEqual(dF * overlap, 0)) {
                d = DIM_MAX;
            }
        }
        if (d != DIM_MAX) {
            distances.push_back(d);
        }
    }

    if (dotF > ABmin && dotF < ABmax) {
        double d = pointDistance(F, A, B, direction);
        if (d != DIM_MAX && FuzzyEqual(d, 0)) { // && crossE<crossF A currently touches EF, but AB is moving away from EF
            double dE = pointDistance(E, A, B, direction, true);
            if (dE < 0 || FuzzyEqual(dE * overlap, 0)) {
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

double polygonSlideDistance(LB_Polygon2D A, LB_Polygon2D B, const LB_Coord2D &direction, bool ignoreNegative)
{
    LB_Coord2D A1, A2, B1, B2;
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

    LB_Polygon2D edgeA = A;
    LB_Polygon2D edgeB = B;

    double distance = DIM_MAX;
    double d;

    LB_Coord2D dir = direction.Normalized();

    for (int i = 0; i < edgeB.size() - 1; i++) {
        for (int j = 0; j < edgeA.size() - 1; j++) {
            A1 = {edgeA[j].X() + Aoffsetx, edgeA[j].Y() + Aoffsety};
            A2 = {edgeA[j+1].X() + Aoffsetx, edgeA[j+1].Y() + Aoffsety};
            B1 = {edgeB[i].X() + Boffsetx, edgeB[i].Y() + Boffsety};
            B2 = {edgeB[i+1].X() + Boffsetx, edgeB[i+1].Y() + Boffsety};

            if((FuzzyEqual(A1.X(), A2.X()) && FuzzyEqual(A1.Y(), A2.Y())) || (FuzzyEqual(B1.X(), B2.X()) && FuzzyEqual(B1.Y(), B2.Y()))) {
                continue; // ignore extremely small lines
            }

            d = segmentDistance(A1, A2, B1, B2, dir);

            if(d != DIM_MAX && (distance == DIM_MAX || d < distance)) {
                if(!ignoreNegative || d > 0 || FuzzyEqual(d, 0.0)) {
                    distance = d;
                }
            }
        }
    }
    return distance;
}

double polygonProjectionDistance(LB_Polygon2D A, LB_Polygon2D B, const LB_Coord2D &direction)
{
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

    LB_Polygon2D edgeA = A;
    LB_Polygon2D edgeB = B;

    double distance = DIM_MAX;
    LB_Coord2D p, s1, s2;
    double d;

    for (int i = 0; i < edgeB.size(); i++) {
        // the shortest/most negative projection of B onto A
        double minprojection = DIM_MAX;
        LB_Coord2D minp = INVALID_POINT;
        for (int j = 0; j < edgeA.size() - 1; j++) {
            p = {edgeB[i].X() + Boffsetx, edgeB[i].Y() + Boffsety};
            s1 = {edgeA[j].X() + Aoffsetx, edgeA[j].Y() + Aoffsety};
            s2 = {edgeA[j+1].X() + Aoffsetx, edgeA[j+1].Y() + Aoffsety};

            if(fabs((s2.Y()-s1.Y()) * direction.X() - (s2.X()-s1.X()) * direction.Y()) < FLOAT_TOL) {
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

bool inNfp(const LB_Coord2D &p, const QVector<LB_Polygon2D> &nfp)
{
    if (nfp.empty()) {
        return false;
    }

    for (int i = 0; i < nfp.size(); i++) {
        for (int j = 0; j < nfp[i].size(); j++) {
            if (FuzzyEqual(p.X(), nfp[i][j].X()) && FuzzyEqual(p.Y(), nfp[i][j].Y())) {
                return true;
            }
        }
    }

    return false;
}

LB_Coord2D searchStartPoint(LB_Polygon2D A, LB_Polygon2D B, bool inside, const QVector<LB_Polygon2D> &NFP)
{
    // close the loop for polygons
    if (A[0] != A[A.size() - 1]) {
        A.push_back(A[0]);
    }

    if (B[0] != B[B.size() - 1]) {
        B.push_back(B[0]);
    }

    for (int i = 0; i < A.size() - 1; i++) {
        if (!A[i].Marked()) {
            A[i].setMarked(true);
            for (int j = 0; j < B.size(); j++) {
                B.offsetx = A[i].X() - B[j].X();
                B.offsety = A[i].Y() - B[j].Y();

                PointInPolygon Binside = PointInPolygon::INVALID;
                for (int k = 0; k < B.size(); k++) {
                    PointInPolygon inpoly = A.ContainPoint({ B[k].X() + B.offsetx,
                                                             B[k].Y() + B.offsety });
                    if (inpoly != PointInPolygon::INVALID) {
                        Binside = inpoly;
                        break;
                    }
                }

                if (Binside == PointInPolygon::INVALID) { // A and B are the same
                    return INVALID_POINT;
                }

                LB_Coord2D startPoint = { B.offsetx, B.offsety };
                if (((Binside && inside) || (!Binside && !inside)) && !A.Intersect(B)
                        && !inNfp(startPoint, NFP)) {
                    return startPoint;
                }

                // slide B along vector
                double vx = A[i + 1].X() - A[i].X();
                double vy = A[i + 1].Y() - A[i].Y();

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
                if (d == DIM_MAX && !FuzzyEqual(d, 0) && d > 0) {

                } else {
                    continue;
                }

                double vd2 = vx * vx + vy * vy;

                if (d * d < vd2 && !FuzzyEqual(d * d, vd2)) {
                    double vd = sqrt(vx * vx + vy * vy);
                    vx *= d / vd;
                    vy *= d / vd;
                }

                B.offsetx += vx;
                B.offsety += vy;

                for (int k = 0; k < B.size(); k++) {
                    PointInPolygon inpoly = A.ContainPoint({ B[k].X() + B.offsetx,
                                                             B[k].Y() + B.offsety });
                    if (inpoly == PointInPolygon::INVALID) {
                        Binside = inpoly;
                        break;
                    }
                }
                startPoint = {B.offsetx, B.offsety};
                if (((Binside && inside) || (!Binside && !inside)) && !A.Intersect(B)
                        && !inNfp(startPoint, NFP)) {
                    return startPoint;
                }
            }
        }
    }

    return INVALID_POINT;
}

QVector<LB_Polygon2D> noFitPolygonRectangle(const LB_Polygon2D &A, const LB_Polygon2D &B)
{
    double minAx = A[0].X();
    double minAy = A[0].Y();
    double maxAx = A[0].X();
    double maxAy = A[0].Y();

    for (int i = 1; i < A.size(); i++) {
        if (A[i].X() < minAx) {
            minAx = A[i].X();
        }
        if (A[i].Y() < minAy) {
            minAy = A[i].Y();
        }
        if (A[i].X() > maxAx) {
            maxAx = A[i].X();
        }
        if (A[i].Y() > maxAy) {
            maxAy = A[i].Y();
        }
    }

    double minBx = B[0].X();
    double minBy = B[0].Y();
    double maxBx = B[0].X();
    double maxBy = B[0].Y();

    for (int i = 1; i < B.size(); i++) {
        if (B[i].X() < minBx) {
            minBx = B[i].X();
        }
        if (B[i].Y() < minBy) {
            minBy = B[i].Y();
        }
        if (B[i].X() > maxBx) {
            maxBx = B[i].X();
        }
        if (B[i].Y() > maxBy) {
            maxBy = B[i].Y();
        }
    }

    if (maxBx - minBx > maxAx - minAx) {
        return {};
    }
    if (maxBy - minBy > maxAy - minAy) {
        return {};
    }

    return { {
            {	minAx-minBx+B[0].X(), minAy-minBy+B[0].Y()},
            {	maxAx-maxBx+B[0].X(), minAy-minBy+B[0].Y()},
            {	maxAx-maxBx+B[0].X(), maxAy-maxBy+B[0].Y()},
            {	minAx-minBx+B[0].X(), maxAy-maxBy+B[0].Y()}
        }};
}

QVector<LB_Polygon2D> noFitPolygon(LB_Polygon2D A, LB_Polygon2D B, bool inside, bool searchEdges)
{
    if(A.size() < 3 || B.size() < 3){
        return {};
    }

    A.offsetx = 0;
    A.offsety = 0;

    int i, j;

    double minA = A[0].Y();
    int minAindex = 0;

    double maxB = B[0].Y();
    int maxBindex = 0;

    for(i=1; i<A.size(); i++){
        A[i].setMarked(false);
        if(A[i].Y() < minA){
            minA = A[i].Y();
            minAindex = i;
        }
    }

    for(i=1; i<B.size(); i++){
        B[i].setMarked(false);
        if(B[i].Y() > maxB){
            maxB = B[i].Y();
            maxBindex = i;
        }
    }

    LB_Coord2D startpoint;
    if(!inside){
        // shift B such that the bottom-most point of B is at the top-most point of A. This guarantees an initial placement with no intersections
        startpoint = {
            A[minAindex].X()-B[maxBindex].X(),
            A[minAindex].Y()-B[maxBindex].Y()
        };
    }
    else{
        // no reliable heuristic for inside
        startpoint = searchStartPoint(A,B,true);
    }

    QVector<LB_Polygon2D> NFPlist;

    struct EdgeDescriptor {
        int8_t type;
        int A;
        int B;
    };

    while(startpoint != INVALID_POINT){
        B.offsetx = startpoint.X();
        B.offsety = startpoint.Y();


        // maintain a list of touching points/edges
        QVector<EdgeDescriptor> touching;

        LB_Coord2D prevvector = INVALID_POINT; // keep track of previous vector
        LB_Polygon2D NFP;
        NFP.push_back({B[0].X()+B.offsetx,B[0].Y()+B.offsety});

        double referencex = B[0].X()+B.offsetx;
        double referencey = B[0].Y()+B.offsety;
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
                    if(FuzzyEqual(A[i].X(), B[j].X()+B.offsetx) && FuzzyEqual(A[i].Y(), B[j].Y()+B.offsety)){
                        touching.push_back({ 0, i, j });
                    }
                    else if(Shape2D::LB_Coord2D::OnSegment(A[i],A[nexti],{B[j].X()+B.offsetx, B[j].Y() + B.offsety})){
                        touching.push_back({	1, nexti, j });
                    }
                    else if(Shape2D::LB_Coord2D::OnSegment({B[j].X()+B.offsetx, B[j].Y() + B.offsety},{B[nextj].X()+B.offsetx, B[nextj].Y() + B.offsety},A[i])){
                        touching.push_back({	2, i, nextj });
                    }
                }
            }


            struct TransVector {
                double x;
                double y;
                LB_Coord2D start;
                LB_Coord2D end;

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
                LB_Coord2D vertexA = A[touching[i].A];
                vertexA.setMarked(true);

                // adjacent A vertices
                int prevAindex = touching[i].A-1;
                int nextAindex = touching[i].A+1;

                prevAindex = (prevAindex < 0) ? A.size()-1 : prevAindex; // loop
                nextAindex = (nextAindex >= A.size()) ? 0 : nextAindex; // loop

                LB_Coord2D prevA = A[prevAindex];
                LB_Coord2D nextA = A[nextAindex];

                // adjacent B vertices
                LB_Coord2D vertexB = B[touching[i].B];

                int prevBindex = touching[i].B-1;
                int nextBindex = touching[i].B+1;

                prevBindex = (prevBindex < 0) ? B.size()-1 : prevBindex; // loop
                nextBindex = (nextBindex >= B.size()) ? 0 : nextBindex; // loop

                LB_Coord2D prevB = B[prevBindex];
                LB_Coord2D nextB = B[nextBindex];

                if(touching[i].type == 0){
                    TransVector vA1 = {
                        prevA.X()-vertexA.X(),
                        prevA.Y()-vertexA.Y(),
                        vertexA,
                        prevA
                    };

                    TransVector vA2 = {
                        nextA.X()-vertexA.X(),
                        nextA.Y()-vertexA.Y(),
                        vertexA,
                        nextA
                    };

                    // B vectors need to be inverted
                    TransVector vB1 = {
                        vertexB.X()-prevB.X(),
                        vertexB.Y()-prevB.Y(),
                        prevB,
                        vertexB
                    };

                    TransVector vB2 = {
                        vertexB.X()-nextB.X(),
                        vertexB.Y()-nextB.Y(),
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
                                          vertexA.X()-(vertexB.X()+B.offsetx),
                                          vertexA.Y()-(vertexB.Y()+B.offsety),
                                          prevA,
                                          vertexA
                                      });

                    vectors.push_back({
                                          prevA.X()-(vertexB.X()+B.offsetx),
                                          prevA.Y()-(vertexB.Y()+B.offsety),
                                          vertexA,
                                          prevA
                                      });
                }
                else if(touching[i].type == 2){
                    vectors.push_back({
                                          vertexA.X()-(vertexB.X()+B.offsetx),
                                          vertexA.Y()-(vertexB.Y()+B.offsety),
                                          prevB,
                                          vertexB
                                      });

                    vectors.push_back({
                                          vertexA.X()-(prevB.X()+B.offsetx),
                                          vertexA.Y()-(prevB.Y()+B.offsety),
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
                if(prevvector != INVALID_POINT && vectors[i].y * prevvector.Y() + vectors[i].x * prevvector.X() < 0){
                    // compare magnitude with unit vectors
                    double vectorlength = sqrt(vectors[i].x*vectors[i].x+vectors[i].y*vectors[i].y);
                    LB_Coord2D unitv = {vectors[i].x/vectorlength, vectors[i].y/vectorlength};

                    double prevlength = sqrt(prevvector.X()*prevvector.X()+prevvector.Y()*prevvector.Y());
                    LB_Coord2D prevunit = {prevvector.X()/prevlength, prevvector.Y()/prevlength};

                    // we need to scale down to unit vectors to normalize vector length. Could also just do a tan here
                    if(fabs(unitv.Y() * prevunit.X() - unitv.X() * prevunit.Y()) < 0.0001){
                        continue;
                    }
                }

                LB_Coord2D pv = {vectors[i].x, vectors[i].y};
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


            if(translate == INVALID_TRANSVECTOR || FuzzyEqual(maxd, 0)){
                // didn't close the loop, something went wrong here
                NFP = {};
                break;
            }

            translate.start.setMarked(true);
            translate.end.setMarked(true);

            prevvector = LB_Coord2D(translate.x, translate.y);

            // trim
            double vlength2 = translate.x*translate.x + translate.y*translate.y;
            if(maxd*maxd < vlength2 && !FuzzyEqual(maxd*maxd, vlength2)){
                double scale = sqrt((maxd*maxd)/vlength2);
                translate.x *= scale;
                translate.y *= scale;
            }

            referencex += translate.x;
            referencey += translate.y;

            if(FuzzyEqual(referencex, startx) && FuzzyEqual(referencey, starty)){
                // we've made a full loop
                break;
            }

            // if A and B start on a touching horizontal line, the end point may not be the start point
            bool looped = false;
            if(!NFP.empty()){
                for(i=0; i<NFP.size()-1; i++){
                    if(FuzzyEqual(referencex, NFP[i].X()) && FuzzyEqual(referencey, NFP[i].Y())){
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

}
