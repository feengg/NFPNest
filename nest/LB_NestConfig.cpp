#include "LB_NestConfig.h"

namespace NestConfig {

double LB_NestConfig::STRIP_WIDTH = 1000;
double LB_NestConfig::STRIP_HEIGHT = 1000;
bool LB_NestConfig::ENABLE_ROTATION = true;
double LB_NestConfig::ITEM_GAP = 0;

QString LB_NestConfig::DumpConfig()
{
    return QString("Strip:(%1 X %2), Enable Rotation:%3, Item Gap:%4").arg(STRIP_WIDTH).arg(STRIP_HEIGHT).arg(ENABLE_ROTATION).arg(ITEM_GAP);
}

}
