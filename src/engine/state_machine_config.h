#ifndef STATE_MACHINE_CONFIG_H
#define STATE_MACHINE_CONFIG_H

#include "enum_config.h"

// state_machine_config's enum values (station_idle, station_active,
// player_engaged, player_disengaged) live in enum_config.h alongside every
// other enum; this header exists so call sites can include the
// state-machine-specific config without pulling in every other domain's enums.
#endif
