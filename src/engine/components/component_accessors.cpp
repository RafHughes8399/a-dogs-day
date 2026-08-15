#include "component.h"

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
bool components::interactable_component::has_free_slot() const{
    return interactors_.size() < capacity_;
}
bool components::interactable_component::add_interactor(size_t entity_id){
    if(has_free_slot()){
        interactors_.push_back(entity_id);
        return true;
    }
    else{return false;}
}
void components::interactable_component::remove_interactor(size_t entity_id){
    std::erase_if(interactors_, [entity_id](size_t entity) -> bool {
        return entity_id == entity;
    });
    
}
// ---------------- interactor components ----------------
Rectangle components::interactor_component::get_interaction_box(Rectangle box) const{
    return Rectangle{box.x - reach_,
                     box.y - reach_,
                     box.width + 2.0f * reach_,
                     box.height + 2.0f * reach_};
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
