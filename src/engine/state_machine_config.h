#ifndef STATE_MACHINE_CONFIG_H
#define STATE_MACHINE_CONFIG_H

#include "events/event_core.h"

namespace state_machine_config{
    enum cook_station_states{
        station_idle = 0,
        station_active,
        station_states_size
    };
    enum state_transitions{
        player_engaged = events::ids::player_engaged_id,
        player_disengaged = events::ids::player_disengaged_id
    };
}
#endif
