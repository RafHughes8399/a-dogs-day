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
        std::for_each(hitboxes_.begin(), hitboxes_.end(), [this](hitbox::hitbox& h) -> void {
            h.update(position_);
        });
    }
    return status_codes::nothing;
}


void entities::player_dog::select(){
    selected_state_ = std::make_unique<selected>();
}
void entities::player_dog::unselect(){
    selected_state_ = std::make_unique<unselected>();
}
void entities::player_dog::selected::render(player_dog& dog){
    // draw the outlines
    dog.outlines_[dog.sprite_index_].render(dog.position_);
}
void entities::player_dog::unselected::render(player_dog& dog){
    // do nothing, already handled
    (void) dog;
    return;
}
void entities::player_dog::render(){
    entity::render();
    selected_state_->render(*this);
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
        sprite_index_ = level_config::directions::right;
        return;
    }
    else if(position_.x > target.x){
        // moving left
        direction_scalar_ = level_config::direction_scalars[level_config::directions::left];
        sprite_index_ = level_config::directions::left;
        return;
    }
    else if(position_.y < target.y){
        // moving down 
        direction_scalar_ = level_config::direction_scalars[level_config::directions::down];
        sprite_index_ = level_config::directions::down;
        return;
    }
    else if(position_.y > target.y){
        // moving up
        direction_scalar_ = level_config::direction_scalars[level_config::directions::up];
        sprite_index_ = level_config::directions::up;
        return;
    }
}
// ------------------------------- builder ------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_khiri(Vector2 position, int id){
    auto khiri_left_texture = textures::textures_.get_texture(textures::khiri_left, assets_config::khiri_left_path);
    auto khiri_right_texture = textures::textures_.get_texture(textures::khiri_right, assets_config::khiri_right_path);
    auto khiri_up_texture = textures::textures_.get_texture(textures::khiri_up, assets_config::khiri_up_path);
    auto khiri_down_texture = textures::textures_.get_texture(textures::khiri_down, assets_config::khiri_down_path);

   auto khiri_left_outline_texture = textures::textures_.get_texture(textures::khiri_left_out, assets_config::khiri_left_outline_path);
   auto khiri_right_outline_texture = textures::textures_.get_texture(textures::khiri_right_out, assets_config::khiri_right_outline_path);
   auto khiri_up_outline_texture = textures::textures_.get_texture(textures::khiri_up_out, assets_config::khiri_up_outline_path);
   auto khiri_down_outline_texture = textures::textures_.get_texture(textures::khiri_down_out, assets_config::khiri_down_outline_path);

    auto khiri_left_sprite = sprite::sprite(khiri_left_texture,
        assets_config::khiri_across_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_across_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_across_attributes[assets_config::attributes::frames],
        assets_config::khiri_across_attributes[assets_config::attributes::animations]);
    
    auto khiri_right_sprite = sprite::sprite(khiri_right_texture,
        assets_config::khiri_across_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_across_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_across_attributes[assets_config::attributes::frames],
        assets_config::khiri_across_attributes[assets_config::attributes::animations]);

    auto khiri_up_sprite = sprite::sprite(khiri_up_texture,
        assets_config::khiri_down_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_down_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_down_attributes[assets_config::attributes::frames],
        assets_config::khiri_down_attributes[assets_config::attributes::animations]);
    
    auto khiri_down_sprite = sprite::sprite(khiri_down_texture,
        assets_config::khiri_down_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_down_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_down_attributes[assets_config::attributes::frames],
        assets_config::khiri_down_attributes[assets_config::attributes::animations]);

    auto khiri_left_outline_sprite = sprite::sprite(khiri_left_outline_texture,
        assets_config::khiri_across_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_across_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_across_attributes[assets_config::attributes::frames],
        assets_config::khiri_across_attributes[assets_config::attributes::animations]);
    
    auto khiri_right_outline_sprite = sprite::sprite(khiri_right_outline_texture,
        assets_config::khiri_across_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_across_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_across_attributes[assets_config::attributes::frames],
        assets_config::khiri_across_attributes[assets_config::attributes::animations]);

    auto khiri_up_outline_sprite = sprite::sprite(khiri_up_outline_texture,
        assets_config::khiri_down_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_down_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_down_attributes[assets_config::attributes::frames],
        assets_config::khiri_down_attributes[assets_config::attributes::animations]);
    
    auto khiri_down_outline_sprite = sprite::sprite(khiri_down_outline_texture,
        assets_config::khiri_down_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_down_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_down_attributes[assets_config::attributes::frames],
        assets_config::khiri_down_attributes[assets_config::attributes::animations]);
       

    auto across_hitbox = hitbox::h_builder_.build_player_dog_across_hitbox(position);
    auto down_hitbox = hitbox::h_builder_.build_player_dog_down_hitbox(position);

    // ! up down left right
    auto khiri_sprites = std::vector<sprite::sprite>{khiri_up_sprite, khiri_down_sprite, khiri_left_sprite, khiri_right_sprite};
    auto khiri_outlines = std::vector<sprite::sprite>{khiri_up_outline_sprite, khiri_down_outline_sprite, khiri_left_outline_sprite, khiri_right_outline_sprite};
    auto khiri_hitboxes = std::vector<hitbox::hitbox>{down_hitbox, down_hitbox, across_hitbox, across_hitbox};

    return std::make_unique<entities::player_dog>(
        khiri_sprites,
        khiri_outlines,
        khiri_hitboxes,
        position,
        id);
}
std::unique_ptr<entities::entity> entities::entity_builder::build_mack(Vector2 position, int id){
    auto mack_left_texture = textures::textures_.get_texture(textures::mack_left, assets_config::mack_left_path);
    auto mack_right_texture = textures::textures_.get_texture(textures::mack_right, assets_config::mack_right_path);
    auto mack_up_texture = textures::textures_.get_texture(textures::mack_up, assets_config::mack_up_path);
    auto mack_down_texture = textures::textures_.get_texture(textures::mack_down, assets_config::mack_down_path);

   auto mack_left_outline_texture = textures::textures_.get_texture(textures::mack_left_out, assets_config::mack_left_outline_path);
   auto mack_right_outline_texture = textures::textures_.get_texture(textures::mack_right_out, assets_config::mack_right_outline_path);
   auto mack_up_outline_texture = textures::textures_.get_texture(textures::mack_up_out, assets_config::mack_up_outline_path);
   auto mack_down_outline_texture = textures::textures_.get_texture(textures::mack_down_out, assets_config::mack_down_outline_path);

    auto mack_left_sprite = sprite::sprite(mack_left_texture,
        assets_config::mack_across_attributes[assets_config::attributes::frame_width],
        assets_config::mack_across_attributes[assets_config::attributes::frame_height],
        assets_config::mack_across_attributes[assets_config::attributes::frames],
        assets_config::mack_across_attributes[assets_config::attributes::animations]);
    
    auto mack_right_sprite = sprite::sprite(mack_right_texture,
        assets_config::mack_across_attributes[assets_config::attributes::frame_width],
        assets_config::mack_across_attributes[assets_config::attributes::frame_height],
        assets_config::mack_across_attributes[assets_config::attributes::frames],
        assets_config::mack_across_attributes[assets_config::attributes::animations]);

    auto mack_up_sprite = sprite::sprite(mack_up_texture,
        assets_config::mack_down_attributes[assets_config::attributes::frame_width],
        assets_config::mack_down_attributes[assets_config::attributes::frame_height],
        assets_config::mack_down_attributes[assets_config::attributes::frames],
        assets_config::mack_down_attributes[assets_config::attributes::animations]);
    
    auto mack_down_sprite = sprite::sprite(mack_down_texture,
        assets_config::mack_down_attributes[assets_config::attributes::frame_width],
        assets_config::mack_down_attributes[assets_config::attributes::frame_height],
        assets_config::mack_down_attributes[assets_config::attributes::frames],
        assets_config::mack_down_attributes[assets_config::attributes::animations]);

    auto mack_left_outline_sprite = sprite::sprite(mack_left_outline_texture,
        assets_config::mack_across_attributes[assets_config::attributes::frame_width],
        assets_config::mack_across_attributes[assets_config::attributes::frame_height],
        assets_config::mack_across_attributes[assets_config::attributes::frames],
        assets_config::mack_across_attributes[assets_config::attributes::animations]);
    
    auto mack_right_outline_sprite = sprite::sprite(mack_right_outline_texture,
        assets_config::mack_across_attributes[assets_config::attributes::frame_width],
        assets_config::mack_across_attributes[assets_config::attributes::frame_height],
        assets_config::mack_across_attributes[assets_config::attributes::frames],
        assets_config::mack_across_attributes[assets_config::attributes::animations]);

    auto mack_up_outline_sprite = sprite::sprite(mack_up_outline_texture,
        assets_config::mack_down_attributes[assets_config::attributes::frame_width],
        assets_config::mack_down_attributes[assets_config::attributes::frame_height],
        assets_config::mack_down_attributes[assets_config::attributes::frames],
        assets_config::mack_down_attributes[assets_config::attributes::animations]);
    
    auto mack_down_outline_sprite = sprite::sprite(mack_down_outline_texture,
        assets_config::mack_down_attributes[assets_config::attributes::frame_width],
        assets_config::mack_down_attributes[assets_config::attributes::frame_height],
        assets_config::mack_down_attributes[assets_config::attributes::frames],
        assets_config::mack_down_attributes[assets_config::attributes::animations]);
       
    auto across_hitbox = hitbox::h_builder_.build_player_dog_across_hitbox(position);
    auto down_hitbox = hitbox::h_builder_.build_player_dog_down_hitbox(position);

    // ! up down left right
    auto mack_sprites = std::vector<sprite::sprite>{mack_up_sprite, mack_down_sprite, mack_left_sprite, mack_right_sprite};
    auto mack_outlines = std::vector<sprite::sprite>{mack_up_outline_sprite, mack_down_outline_sprite, mack_left_outline_sprite, mack_right_outline_sprite};
    auto mack_hitboxes = std::vector<hitbox::hitbox>{down_hitbox, down_hitbox, across_hitbox, across_hitbox};


    return std::make_unique<entities::player_dog>(
        mack_sprites,
        mack_outlines,
        mack_hitboxes,
        position,
        id);
}