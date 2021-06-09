#ifndef LB_NFPHANDLE_H
#define LB_NFPHANDLE_H

#include "LB_Polygon2D.h"
using namespace Shape2D;

namespace LB_NFP {

double pointDistance(const LB_Coord2D& p, const LB_Coord2D& s1, const LB_Coord2D& s2,
                    LB_Coord2D normal, bool infinite = false);

double segmentDistance(const LB_Coord2D& A, const LB_Coord2D& B, const LB_Coord2D& E,
                      const LB_Coord2D& F, const LB_Coord2D& direction);

double polygonSlideDistance(LB_Polygon2D A, LB_Polygon2D B, const LB_Coord2D& direction,
                           bool ignoreNegative);

double polygonProjectionDistance(LB_Polygon2D A, LB_Polygon2D B, const LB_Coord2D& direction);

// returns true if point already exists in the given nfp
bool inNfp(const LB_Coord2D& p, const QVector<LB_Polygon2D>& nfp);

LB_Coord2D searchStartPoint(LB_Polygon2D A, LB_Polygon2D B, bool inside,
                       const QVector<LB_Polygon2D>& NFP = {});

// returns an interior NFP for the special case where A is a rectangle
QVector<LB_Polygon2D> noFitPolygonRectangle(const LB_Polygon2D& A, const LB_Polygon2D& B);

// given a static polygon A and a movable polygon B, compute a no fit polygon by orbiting B about A
// if the inside flag is set, B is orbited inside of A rather than outside
// if the searchEdges flag is set, all edges of A are explored for NFPs - multiple
QVector<LB_Polygon2D> noFitPolygon(LB_Polygon2D A, LB_Polygon2D B, bool inside, bool searchEdges);

}

#endif // LB_NFPHANDLE_H
