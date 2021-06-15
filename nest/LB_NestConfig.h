#ifndef LB_NESTCONFIG_H
#define LB_NESTCONFIG_H

#include <QString>

namespace NestConfig {

class LB_NestConfig
{
public:
    static double STRIP_WIDTH;
    static double STRIP_HEIGHT;
    static bool ENABLE_ROTATION;
    static double ITEM_GAP;

    static QString DumpConfig();

private:
    LB_NestConfig() {}
};

}

#endif // LB_NESTCONFIG_H
