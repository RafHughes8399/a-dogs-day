#include "entities.h"

// ------------------------------- player dogs ------------------------------- //
int entities::player_dog::update(float delta){
    (void) delta;
    /**
     * the dog should store its move path
     * if the dog path is empty, then the dog is not moving,
     * for now do nothing 
     */
    // process the head of the move path if it exists,

    // need an iniital setting of the direction
    draw_path();
    if(! move_path_.empty()){
        auto next_position = move_path_.front();
        if(reached_position(next_position)){
            move_path_.erase(move_path_.begin()); // the direction of movement should be the same until a new position in the path is reached
            if(! move_path_.empty()){
                next_position = move_path_.front();
                determine_direction(next_position);
            }
        }
        auto new_position = Vector2Add(position_, Vector2Scale(Vector2Multiply(move_speed_, direction_scalar_), delta));
        position_ = new_position;
        // update the hitbox too 
        hitbox_.update(position_);
    }
    return status_codes::nothing;
}

Vector2 entities::player_dog::get_direction_scalar(){
    return direction_scalar_;
}
void entities::player_dog::interact(entity& other){
    (void) other;
    return;
}
void entities::player_dog::on_right_click_event(const events::right_mouse_click& event){
    auto destination = event.get_mouse_position();
    // update the dog's movement path, query the level graph
    (void) destination;
    return;
}

void entities::player_dog::set_path(std::vector<Vector2>& path){
    move_path_ = path;
    // when the path is set, also pick a direction 
    // compare the position with the next position in the path
    determine_direction(move_path_.front());
}
void entities::player_dog::draw_path(){
    for(auto position : move_path_){
        int row = position.y / level_config::edge_weight;
        int row_length = level_config::world_x / level_config::edge_weight;
        int col = position.x / level_config::edge_weight;
        int index = (row * row_length) + col;
        DrawCircle(position.x, position.y, 15, YELLOW);
        DrawText(TextFormat("%d", index), position.x, position.y, 12, WHITE);
    }
}
bool entities::player_dog::reached_position(Vector2 target){

    float position_to_target = Vector2Distance(position_, target);
    // which is 3.2
    if(position_to_target <= level_config::edge_weight * 0.05){
        position_ = target;
        return true;
    } 
    return false;
}

void entities::player_dog::determine_direction(Vector2 target){
    // it is either up down left or right
    // x is left right, y is up down 
    if(position_.x < target.x){
        // moving right
        direction_scalar_ = level_config::direction_scalars[level_config::directions::right];
        return;
    }
    else if(position_.x > target.x){
        // moving left
        direction_scalar_ = level_config::direction_scalars[level_config::directions::left];
        return;
    }
    else if(position_.y < target.y){
        // moving down 
        direction_scalar_ = level_config::direction_scalars[level_config::directions::down];
        return;
    }
    else if(position_.y > target.y){
        // moving up
        direction_scalar_ = level_config::direction_scalars[level_config::directions::up];
        return;
    }
}
// ------------------------------- builder ------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_khiri(Vector2 position, int id){
    auto khiri_texture = textures::textures_.get_texture(textures::khiri, assets_config::khiri_path);
    auto hitbox = hitbox::h_builder_.build_player_dog_hitbox(position);
    return std::make_unique<entities::player_dog>(
        sprite::sprite(khiri_texture,
            assets_config::khiri_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_attributes[assets_config::attributes::frames],
        assets_config::khiri_attributes[assets_config::attributes::animations]),
        hitbox,
        position,
        id);
}
std::unique_ptr<entities::entity> entities::entity_builder::build_mack(Vector2 position, int id){
    auto mack_texture = textures::textures_.get_texture(textures::mack, assets_config::mack_path);
    auto hitbox = hitbox::h_builder_.build_player_dog_hitbox(position);
    return std::make_unique<entities::player_dog>(
        sprite::sprite(mack_texture,
        assets_config::mack_attributes[assets_config::attributes::frame_width],
        assets_config::mack_attributes[assets_config::attributes::frame_height],
        assets_config::mack_attributes[assets_config::attributes::frames],
        assets_config::mack_attributes[assets_config::attributes::animations]),
        hitbox,
        position,
        id); // becayse tge move speed is zero
}