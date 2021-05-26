#pragma once

#include "Shapes2D.h"

namespace QGeometryUtil {

enum PointInPolygonResult {
    INSIDE,
    OUTSIDE,
    INVALID
};

bool AlmostEqual(double a, double b, double tolerance = FLOAT_TOL);
bool WithinDistance(const Point& p1, const Point& p2, double distance);
Point NormalizeVec(const Point& v);
bool OnSegment(const Point& A,const Point& B, const Point& p);
Point LineIntersect(const Point& A,const Point& B,const Point& E,const Point& F, bool infinite = false);
PointInPolygonResult InPolygon(const Point& point, const Polygon& polygon);
bool Intersect(const Polygon& A, const Polygon& B);
std::vector<Point> PolygonEdge(const Polygon& polygon, Point normal);
double pointLineDistance(const Point& p, const Point& s1, const Point& s2, Point normal, bool s1inclusive, bool s2inclusive);
double pointDistance(const Point& p, const Point& s1, const Point& s2, Point normal, bool infinite = false);
double segmentDistance(const Point& A, const Point& B, const Point& E, const Point& F, const Point& direction);
double polygonSlideDistance(Polygon A, Polygon B, const Point& direction, bool ignoreNegative);
double polygonProjectionDistance(Polygon A, Polygon B, const Point& direction);
// returns true if point already exists in the given nfp
bool inNfp(const Point& p, const QVector<Polygon> &nfp);
Point searchStartPoint(Polygon A, Polygon B, bool inside, const QVector<Polygon>& NFP = {});
bool isRectangle(const Polygon& poly, double tolerance = FLOAT_TOL) ;
// returns an interior NFP for the special case where A is a rectangle
QVector<Polygon> noFitPolygonRectangle(const Polygon& A, const Polygon& B);

// given a static polygon A and a movable polygon B, compute a no fit polygon by orbiting B about A
	// if the inside flag is set, B is orbited inside of A rather than outside
	// if the searchEdges flag is set, all edges of A are explored for NFPs - multiple
QVector<Polygon> noFitPolygon(Polygon A, Polygon B, bool inside, bool searchEdges);

// given two polygons that touch at at least one point, but do not intersect. Return the outer perimeter of both polygons as a single continuous polygon
// A and B must have the same winding direction
Polygon polygonHull(Polygon A, Polygon B);
Polygon polygonUnion(Polygon A, Polygon B);
} //GeometryUtil
