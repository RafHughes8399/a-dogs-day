#include "entities.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include <vector>
// ------------------------------- render states ------------------------------- //
void entities::player_dog::selected::render(player_dog& dog, Vector2 draw_position, int frame){
    size_t render_index = dog.get_body().get_index();
    dog.outlines_[render_index].render(draw_position, frame);
}

void entities::player_dog::unselected::render(player_dog& dog, Vector2 draw_position, int frame){
    (void) dog;
    (void) draw_position;
    (void) frame;
    return;
}

// ------------------------------- player dogs ------------------------------- //
void entities::player_dog::interact(entity& other){
    (void) other;
    return;
}
void entities::player_dog::on_dog_select_event(const events::selected_dog& event){
    auto dog_id = static_cast<int>(event.get_id());
    if(dog_id == id_){
        select();
    }
    else{
        unselect();
    }
}
void entities::player_dog::on_right_click_event(const events::right_mouse_click& event){
    auto destination = event.get_mouse_position();
    // update the dog's movement path, query the level graph
    (void) destination;
    return;
}

void entities::player_dog::render(Vector2 draw_position, int frame){
    dog::render(draw_position, frame);
    selected_state_->render(*this, draw_position, frame);
}
void entities::player_dog::select(){
    selected_state_ = std::make_unique<selected>();
}
void entities::player_dog::unselect(){
    selected_state_ = std::make_unique<unselected>();
}
