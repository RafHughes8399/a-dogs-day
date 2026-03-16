#include "entities.h"
#include "texture.h"
#include <iostream>
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
void entities::player_dog::select(){
    selected_state_ = std::make_unique<selected>();
}
void entities::player_dog::unselect(){
    selected_state_ = std::make_unique<unselected>();
}
// ------------------------------- player dogs ------------------------------- //
bool entities::player_dog::reached_position(Vector2 target){
    float position_to_target = Vector2Distance(position_, target);
    if(position_to_target <= level_config::edge_weight * 0.05){
        std::cout << "[reached_position] id: " << id_ << " reached (" << target.x << ", " << target.y << ") dist: " << position_to_target << " remaining path: " << move_path_.size() << std::endl;
        position_ = target;
        return true;
    } 
    return false;
}

int entities::player_dog::update(float delta, int frame){
    (void) frame;
    if(! move_path_.empty()){
        auto next_position = move_path_.front();
        if(reached_position(next_position)){
            move_path_.erase(move_path_.begin());
            if(! move_path_.empty()){
                next_position = move_path_.front();
                std::cout << "[update] id: " << id_ << " moving to next waypoint: (" << next_position.x << ", " << next_position.y << ") path remaining: " << move_path_.size() << std::endl;
                determine_direction(next_position);
            } else {
                std::cout << "[update] id: " << id_ << " path exhausted" << std::endl;
            }
        }
        auto new_position = Vector2Add(position_, Vector2Scale(Vector2Multiply(move_speed_, direction_scalar_), delta));
        position_ = new_position;
        body_.update_hitboxes(position_);
    }
    return status_codes::nothing;
}

Vector2 entities::player_dog::get_direction_scalar(){
    return direction_scalar_;
}
void entities::player_dog::determine_direction(Vector2 target){
    std::cout << "[determine_direction] id: " << id_ << " pos: (" << position_.x << ", " << position_.y << ") target: (" << target.x << ", " << target.y << ")" << std::endl;
    std::cout << "[determine_direction] body sprites: " << body_.get_sprites().size() << " body hitboxes: " << body_.get_hitboxes().size() << " current index: " << body_.get_index() << std::endl;
    std::cout << "[determine_direction] outlines size: " << outlines_.size() << std::endl;
    if(position_.x < target.x){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::right];
        std::cout << "[determine_direction] -> RIGHT set_index(" << level_config::directions::right << ") scalar: (" << direction_scalar_.x << ", " << direction_scalar_.y << ")" << std::endl;
        body_.set_index(level_config::directions::right);
        return;
    }
    else if(position_.x > target.x){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::left];
        std::cout << "[determine_direction] -> LEFT set_index(" << level_config::directions::left << ") scalar: (" << direction_scalar_.x << ", " << direction_scalar_.y << ")" << std::endl;
        body_.set_index(level_config::directions::left);
        return;
    }
    else if(position_.y < target.y){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::down];
        std::cout << "[determine_direction] -> DOWN (no index change) scalar: (" << direction_scalar_.x << ", " << direction_scalar_.y << ")" << std::endl;
        return;
    }
    else if(position_.y > target.y){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::up];
        std::cout << "[determine_direction] -> UP (no index change) scalar: (" << direction_scalar_.x << ", " << direction_scalar_.y << ")" << std::endl;
        return;
    }
    std::cout << "[determine_direction] -> NO DIRECTION (pos == target)" << std::endl;
}

void entities::player_dog::interact(entity& other){
    (void) other;
    return;
}
void entities::player_dog::on_dog_select_event(const events::selected_dog& event){
    auto dog_id = event.get_id();
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
    entity::render(draw_position, frame);
    auto draw_position_offset = draw_position;
    head_.render(draw_position, frame);
    selected_state_->render(*this, draw_position, frame);
}

void entities::player_dog::set_path(std::vector<Vector2>& path){
    std::cout << "[set_path] id: " << id_ << " path size: " << path.size() << std::endl;
    if(path.empty()){
        std::cout << "[set_path] empty path, skipping" << std::endl;
        return;
    }
    move_path_ = path;
    std::cout << "[set_path] first waypoint: (" << move_path_.front().x << ", " << move_path_.front().y << ")" << std::endl;
    determine_direction(move_path_.front());
    std::cout << "[set_path] complete" << std::endl;
}



// ------------------------------- builder ------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_khiri(Vector2 position, int id){
    auto khiri_left_texture = textures::textures_.get_texture(textures::khiri_left, entity_config::khiri_left_path);
    auto khiri_right_texture = textures::textures_.get_texture(textures::khiri_right, entity_config::khiri_right_path);

    auto khiri_left_outline_texture = textures::textures_.get_texture(textures::khiri_left_out, entity_config::khiri_left_outline_path);
    auto khiri_right_outline_texture = textures::textures_.get_texture(textures::khiri_right_out, entity_config::khiri_right_outline_path);

    auto khiri_left_sprite = sprite::sprite(khiri_left_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);
    
    auto khiri_right_sprite = sprite::sprite(khiri_right_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);

    auto khiri_left_outline_sprite = sprite::sprite(khiri_left_outline_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);
    
    auto khiri_right_outline_sprite = sprite::sprite(khiri_right_outline_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);


    auto across_hitbox = hitbox::h_builder_.build_player_dog_across_hitbox(position);

    // ! up down left right
    std::vector<sprite::sprite> khiri_sprites;
    khiri_sprites.push_back(std::move(khiri_left_sprite));
    khiri_sprites.push_back(std::move(khiri_right_sprite));

    std::vector<sprite::sprite> khiri_outlines;
    khiri_outlines.push_back(std::move(khiri_left_outline_sprite));
    khiri_outlines.push_back(std::move(khiri_right_outline_sprite));

    std::vector<hitbox::hitbox> khiri_hitboxes;
    khiri_hitboxes.push_back(across_hitbox);
    khiri_hitboxes.push_back(across_hitbox);

    auto body = body::body(khiri_hitboxes, khiri_sprites);
    auto head = body::body(); // TODO fill !
    return std::make_unique<entities::player_dog>(
        std::move(body),
        std::move(head),
        std::move(khiri_outlines),
        position,
        id);
}
std::unique_ptr<entities::entity> entities::entity_builder::build_mack(Vector2 position, int id){
    auto mack_left_texture = textures::textures_.get_texture(textures::mack_left, entity_config::mack_left_path);
    auto mack_right_texture = textures::textures_.get_texture(textures::mack_right, entity_config::mack_right_path);

   auto mack_left_outline_texture = textures::textures_.get_texture(textures::mack_left_out, entity_config::mack_left_outline_path);
   auto mack_right_outline_texture = textures::textures_.get_texture(textures::mack_right_out, entity_config::mack_right_outline_path);


    auto mack_left_sprite = sprite::sprite(mack_left_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);
    
    auto mack_right_sprite = sprite::sprite(mack_right_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);

    auto mack_left_outline_sprite = sprite::sprite(mack_left_outline_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);
    
    auto mack_right_outline_sprite = sprite::sprite(mack_right_outline_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        static_cast<int>(entity_config::mack_across_attributes[entity_config::attributes::frames]),
        static_cast<int>(entity_config::mack_across_attributes[entity_config::attributes::animations]));

    auto across_hitbox = hitbox::h_builder_.build_player_dog_across_hitbox(position);

    // ! up down left right
    std::vector<sprite::sprite> mack_sprites;
    mack_sprites.push_back(std::move(mack_left_sprite));
    mack_sprites.push_back(std::move(mack_right_sprite));

    std::vector<sprite::sprite> mack_outlines;
    mack_outlines.push_back(std::move(mack_left_outline_sprite));
    mack_outlines.push_back(std::move(mack_right_outline_sprite));

    std::vector<hitbox::hitbox> mack_hitboxes;
    mack_hitboxes.push_back(across_hitbox);
    mack_hitboxes.push_back(across_hitbox);

    auto body = body::body(mack_hitboxes, mack_sprites);
    auto head = body::body(); // TODO fill ! and build the head sprites and hitboxes

    // and build the head, pending
    return std::make_unique<entities::player_dog>(
        std::move(body),
        std::move(head),
        std::move(mack_outlines),
        position,
        id);
}