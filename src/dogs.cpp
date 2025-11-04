#include "entities.h"

// ------------------------------- player dogs ------------------------------- //
int entities::player_dog::update(float delta){
    (void) delta;
    /**
     * the dog should store its move path
     * if the dog path is empty, then the dog is not moving
     */
    return status_codes::nothing;
}
void entities::player_dog::interact(entity& other){
    (void) other;
    return;
}
void on_right_click_event(const events::right_mouse_click& event){
    auto destination = event.get_mouse_position();
    // update the dog's movement path, query the level graph
    (void) destination;
    return;
}

// ------------------------------- builder ------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_mack(Vector2 position, int id){
    (void) position;
    (void) id;
    return nullptr;
}
std::unique_ptr<entities::entity> entities::entity_builder::build_khiri(Vector2 position, int id){
    (void) position;
    (void) id;
    return nullptr;
}