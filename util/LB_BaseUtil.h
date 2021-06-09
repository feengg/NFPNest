#ifndef LB_BASEUTIL_H
#define LB_BASEUTIL_H

#include <limits>

namespace BaseUtil {

static const double DIM_MAX = std::numeric_limits<double>::max();
static const double DIM_MIN = std::numeric_limits<double>::min();
static const double FLOAT_TOL = 1e-9;
static const double DEG2RAD = 0.017453293f;
static const double RAD2DEG = 57.2957796f;

static bool FuzzyEqual(double a, double b, double tolerance = FLOAT_TOL) {
    return fabs(a-b) < tolerance;
}

}

#endif // LB_BASEUTIL_H
