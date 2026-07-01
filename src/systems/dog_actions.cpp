#include "dog_actions.h"
#include "events_interface.h"

void dog_actions::send_dog_to_furniture(int dog_id, Vector2 destination, int furniture_id, Vector2 furniture_position){
    std::unique_ptr<events::event> send_dog_event = std::make_unique<events::send_dog_to_furniture>(dog_id, destination, furniture_id, furniture_position);
    event_interface::queue_event(send_dog_event);
    return;
}
void dog_actions::send_dog_to_position(int dog_id, Vector2 position){
    std::unique_ptr<events::event> send_customer_to_position = std::make_unique<events::send_customer_to_position>(dog_id, position);
    event_interface::queue_event(send_customer_to_position);
    return;
}