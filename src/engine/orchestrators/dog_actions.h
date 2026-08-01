#ifndef DOG_ACTIONS_H
#define DOG_ACTIONS_H

#include "events.h"
#include "raylib.h"
#include <memory>

namespace dog_actions {
    void send_dog_to_station(int dog_id, Vector2 destination, int station_id, Vector2 station_position);
    void send_dog_to_position(int dog_id, Vector2 position);
}
#endif