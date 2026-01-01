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
        std::cout << "not empty, get next position " << std::endl;
        auto next_position = move_path_.front();
        std::cout << "position :  " << position_.x << ", " << position_.y << std::endl;
        if(reached_position(next_position)){
            std::cout << "reached point on path " << std::endl;
            move_path_.erase(move_path_.begin()); // the direction of movement should be the same until a new position in the path is reached
            if(! move_path_.empty()){
                next_position = move_path_.front();
                determine_direction(next_position);
            }
        }
        auto new_position = Vector2Add(position_, Vector2Scale(Vector2Multiply(move_speed_, direction_scalar_), delta));
        std::cout << "new_position :  " << new_position.x << ", " << new_position.y << std::endl;
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
bool entities::player_dog::reached_position(Vector2 target){

    std::cout << " new : " << position_.x << ", " << position_.y << std::endl;
    std::cout << " target : " << target.x << ", " << target.y << std::endl;
    float position_to_target = Vector2Distance(position_, target);
    std::cout << "distance between position and target: " << position_to_target << std::endl; 
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
    std::cout << "determine direction " << std::endl;
    std::cout << "position is " << position_.x << ", " << position_.y << std::endl;
    std::cout << "target is " << target.x << ", " << target.y << std::endl;
    if(position_.x < target.x){
        // moving right
        std::cout << " pos x < target x " << std::endl;
        direction_scalar_ = level_config::direction_scalars[level_config::directions::right];
        return;
    }
    else if(position_.x > target.x){
        std::cout << " pos x > target x " << std::endl;
        // moving left
        direction_scalar_ = level_config::direction_scalars[level_config::directions::left];
        return;
    }
    else if(position_.y < target.y){
        std::cout << " pos y < target y " << std::endl;
        // moving down 
        direction_scalar_ = level_config::direction_scalars[level_config::directions::down];
        return;
    }
    else if(position_.y > target.y){
        // moving up
        std::cout << " pos y > target y " << std::endl;
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