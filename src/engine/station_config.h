#ifndef STATION_CONFIG_H
#define STATION_CONFIG_H

#include "cafe_config.h"

namespace station_config{
    inline const float station_reach = level_config::edge_weight * 0.25f;
}
#endif
