#include "component.h"
#include "config.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include "system.h"

// definitions for the accessors declared on each component in component.h

// ---------------- collision_component ----------------
std::vector<hitbox::hitbox>& components::collision_component::hitbox_component::get_hitboxes(){
    return hitboxes_;
}
hitbox::hitbox& components::collision_component::hitbox_component::get_hitbox(){
    return hitboxes_[hitbox_index_];
}
size_t components::collision_component::hitbox_component::get_hitbox_index() const{
    return hitbox_index_;
}
size_t components::collision_component::hitbox_component::num_hitboxes() const{
    return hitboxes_.size();
}
void components::collision_component::hitbox_component::set_index(size_t index){
    hitbox_index_ = index;
}

components::collision_component::hitbox_component& components::collision_component::get_hitbox_component(){
    return hitbox_component_;
}

// ---------------- input components ----------------
std::vector<game_config::input>& components::key_input_component::get_inputs(){
    return controls_;
}
std::vector<game_config::input>& components::mouse_input_component::get_inputs(){
    return inputs_;
}

// ---------------- interactable components ----------------
Rectangle components::interactable_component::get_interaction_box(Rectangle box) const{
    return Rectangle{box.x - reach_,
                     box.y - reach_,
                     box.width + 2.0f * reach_,
                     box.height + 2.0f * reach_};
}
// ---------------- interactor components ----------------
Rectangle components::interactor_component::get_interaction_box(Rectangle box) const{
    return Rectangle{box.x - reach_,
                     box.y - reach_,
                     box.width + 2.0f * reach_,
                     box.height + 2.0f * reach_};
}
std::optional<Vector2> components::interactable_component::get_interaction_offset(Vector2 source, Vector2 own_position) const{
    static const char* direction_names[DIRECTIONS] = {"left", "right", "up", "down"};
    std::optional<size_t> closest_index = std::nullopt;
    float closest_distance = 0.0f;
    for(size_t i = 0; i < positions_.size(); ++i){
        if(not positions_[i].has_value()){ continue; }
        auto offset = positions_[i].value();
        auto position = Vector2Add(own_position, offset);
        auto* node = systems::movement_system::get_instance().node_at(position);
        if(node == nullptr){
            debug::log("[interactable_component::get_interaction_offset] direction: "
                + std::string(direction_names[i]) + ", position: " + raglib::vector_to_string(position)
                + ", REJECTED - off the walkable grid");
            continue;
        }
        if(not node->entities_.empty()){
            std::string occupants;
            for(auto occupant : node->entities_){
                occupants += (occupants.empty() ? "" : ", ") + std::to_string(occupant);
            }
            debug::log("[interactable_component::get_interaction_offset] direction: "
                + std::string(direction_names[i]) + ", position: " + raglib::vector_to_string(position)
                + ", node_id: " + std::to_string(node->id_) + ", REJECTED - occupied by entity "
                + occupants);
            continue;
        }
        auto distance = Vector2Distance(source, position);
        debug::log("[interactable_component::get_interaction_offset] direction: "
            + std::string(direction_names[i]) + ", position: " + raglib::vector_to_string(position)
            + ", node_id: " + std::to_string(node->id_) + ", distance_from_source: " + std::to_string(distance));
        if(not closest_index.has_value() or distance < closest_distance){
            closest_index = i;
            closest_distance = distance;
        }
    }
    std::optional<Vector2> closest_offset = closest_index.has_value()
        ? positions_[closest_index.value()]
        : std::nullopt;
    debug::log(closest_offset.has_value()
        ? "[interactable_component::get_interaction_offset] chosen offset: " + raglib::vector_to_string(closest_offset.value())
        : "[interactable_component::get_interaction_offset] no free interaction slot found");
    return closest_offset;
}

std::optional<size_t> components::interactor_component::get_target() const{
    return target_;
}
void components::interactor_component::interact_with(size_t entity_id){
    target_ = entity_id;
}
void components::interactor_component::stop_interacting(){
    target_ = std::nullopt;
}
// ---------------- movement_component ----------------
bool components::movement_component::has_reached_position(Vector2 position){
    if(paths_.empty()){
        return false;
    }
    float position_to_target = Vector2Distance(position, paths_.front().get_next_position());
    return position_to_target <= level_config::edge_weight * 0.05f;
}
path::path& components::movement_component::get_current_path(){
    return paths_.empty() ? path::empty_path : paths_.front();
}
Vector2 components::movement_component::get_move_speed(){
    return move_speed_;
}
Vector2 components::movement_component::get_direction_scalar(){
    return direction_scalar_;
}
void components::movement_component::set_direction_scalar(Vector2 direction_scalar){
    direction_scalar_ = direction_scalar;
}
std::queue<path::path>& components::movement_component::get_paths(){
    return paths_;
}
void components::movement_component::append_path(path::path path){
    paths_.push(path);
    return;
}
void components::movement_component::set_path(path::path path){
    if(paths_.empty()){
        append_path(path);
    }
    else{
        // override the current path
        clear_paths();
        paths_.push(path);
    }
}
void components::movement_component::clear_paths(){
    paths_ = {};
}
void components::movement_component::finish_path(){
    if(paths_.empty()){ return; }
    paths_.pop();
}

// ---------------- position_component ----------------
Vector2 components::position_component::get_position(){
    return position_;
}
void components::position_component::set_position(Vector2 position){
    position_ = position;
}

// ---------------- renderable_component ----------------
sprite::sprite& components::renderable_component::sprite_component::get_sprite(){
    return sprites_[sprite_index_];
}
size_t components::renderable_component::sprite_component::get_sprite_index() const{
    return sprite_index_;
}
size_t components::renderable_component::sprite_component::num_sprites() const{
    return sprites_.size();
}
void components::renderable_component::sprite_component::set_index(size_t index){
    sprite_index_ = index;
}

std::vector<components::renderable_component::sprite_component>& components::renderable_component::get_sprites(){
    return sprites_;
}

// ---------------- selectable_component ----------------
bool components::selectable_component::is_selected() const{
    return is_selected_;
}
size_t components::selectable_component::get_kind() const{
    return kind_;
}
void components::selectable_component::select(){
    is_selected_ = true;
}
void components::selectable_component::unselect(){
    is_selected_ = false;
}
