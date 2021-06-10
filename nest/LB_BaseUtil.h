#ifndef LB_BASEUTIL_H
#define LB_BASEUTIL_H

#include <limits>
#include <QRandomGenerator>
#include <QColor>

namespace BaseUtil {

static const double DIM_MAX = std::numeric_limits<double>::max();
static const double DIM_MIN = std::numeric_limits<double>::min();
static const double FLOAT_TOL = 1e-9;
static const double DEG2RAD = 0.017453293f;
static const double RAD2DEG = 57.2957796f;

static bool FuzzyEqual(double a, double b, double tolerance = FLOAT_TOL) {
    return fabs(a-b) < tolerance;
}

static int randInt(const int &min, const int &max) {
     return QRandomGenerator::global()->bounded(min,max);
}

static QColor randomColor() {
    int rand[3] = {0};
    for(int i=0;i<3;++i)
        rand[i] = randInt(0,255);

    return QColor(rand[0],rand[1],rand[2]);
}

}

#endif // LB_BASEUTIL_H
