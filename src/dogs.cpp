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
    std::cout << " move path size " << move_path_.size() << std::endl;
    draw_path();
    if(! move_path_.empty()){
        auto next_position = move_path_.front();
        auto new_position = Vector2Scale(Vector2Add(position_, Vector2Multiply(move_speed_, direction_scalar_)), delta);
        if(reached_position(new_position, next_position)){
            move_path_.erase(move_path_.begin()); // the direction of movement should be the same until a new position in the path is reached
            if(! move_path_.empty()){
                determine_direction(next_position);
            }
        }
        position_ = new_position;
    }
    return status_codes::nothing;
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
    std::cout << " set dog path " << std::endl;
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
bool entities::player_dog::reached_position(Vector2 new_position, Vector2 target){
    // reached is either equal or exceeded position
    // maybe more complicated, comapre distances
    
    // ? compare the distances 
    // ? the distance between position and new_position ?
    // ? and the distance between position and target ?

    // ? if the same, then the target is reached, if "less" then new position is not yet at the target
    // ? if greater, then the new position is beyond the target and it has been reached
    float position_to_new = std::abs(Vector2Distance(position_, new_position));
    float position_to_target = std::abs(Vector2Distance(position_, target));
    return position_to_new >= position_to_target;
}

void entities::player_dog::determine_direction(Vector2 target){
    // it is either up down left or right
    // x is left right, y is up down 
    if(position_.x < target.x){
        // moving right
        direction_scalar_ = level_config::direction_scalars[level_config::directions::right];
    }
    else if(position_.x > target.x){
        // moving left
        direction_scalar_ = level_config::direction_scalars[level_config::directions::left];
    }
    else if(position_.y < target.y){
        // moving down 
        direction_scalar_ = level_config::direction_scalars[level_config::directions::down];
    }
    else if(position_.y > target.y){
        // moving up
        direction_scalar_ = level_config::direction_scalars[level_config::directions::up];
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