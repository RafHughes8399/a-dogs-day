#ifndef DOG_ACTIONS_H
#define DOG_ACTIONS_H

#include "events.h"
#include "raylib.h"
#include <memory>

namespace dog_actions {
    void send_dog_to_furniture(int dog_id, Vector2 destination, int furniture_id, Vector2 furniture_position);
    void send_dog_to_position(int dog_id, Vector2 position);
}
#endif