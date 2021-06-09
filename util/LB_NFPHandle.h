#ifndef LB_NFPHANDLE_H
#define LB_NFPHANDLE_H

#include "LB_Polygon2D.h"
using namespace Shape2D;

namespace LB_NFP {

double pointDistance(const Point& p, const Point& s1, const Point& s2,
                    Point normal, bool infinite = false);

double segmentDistance(const Point& A, const Point& B, const Point& E,
                      const Point& F, const Point& direction);

double polygonSlideDistance(Polygon A, Polygon B, const Point& direction,
                           bool ignoreNegative);

double polygonProjectionDistance(Polygon A, Polygon B, const Point& direction);

// returns true if point already exists in the given nfp
bool inNfp(const Point& p, const QVector<Polygon>& nfp);

Point searchStartPoint(Polygon A, Polygon B, bool inside,
                       const QVector<Polygon>& NFP = {});

// returns an interior NFP for the special case where A is a rectangle
QVector<Polygon> noFitPolygonRectangle(const Polygon& A, const Polygon& B);

// given a static polygon A and a movable polygon B, compute a no fit polygon by orbiting B about A
// if the inside flag is set, B is orbited inside of A rather than outside
// if the searchEdges flag is set, all edges of A are explored for NFPs - multiple
QVector<Polygon> noFitPolygon(Polygon A, Polygon B, bool inside, bool searchEdges);

}

#endif // LB_NFPHANDLE_H
