#include "dog_actions.h"
#include "events_interface.h"

void dog_actions::send_dog_to_station(int dog_id, Vector2 destination, int station_id, Vector2 station_position){
    std::unique_ptr<events::event> send_dog_event = std::make_unique<events::send_dog_to_station>(dog_id, destination, station_id, station_position);
    event_interface::queue_event(send_dog_event);
    return;
}
void dog_actions::send_dog_to_position(int dog_id, Vector2 position){
    std::unique_ptr<events::event> send_dog_to_position = std::make_unique<events::send_dog_to_position>(dog_id, position);
    event_interface::queue_event(send_dog_to_position);
    return;
}