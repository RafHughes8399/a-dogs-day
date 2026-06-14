#include "entities.h"
#include "texture.h"
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
// ------------------------------- dogs ------------------------------- //
bool entities::dog::reached_position(Vector2 target){
    float position_to_target = Vector2Distance(position_, target);
    if(position_to_target <= level_config::edge_weight * 0.05){
        position_ = target;
        return true;
    } 
    return false;
}

int entities::dog::update(float delta, int frame){
    (void) frame;
    if(! move_path_.empty()){
        auto next_position = move_path_.front();
        if(reached_position(next_position)){
            move_path_.erase(move_path_.begin());
            if(! move_path_.empty()){
                next_position = move_path_.front();
                determine_direction(next_position);
            }
        }
        auto new_position = Vector2Add(position_, Vector2Scale(Vector2Multiply(move_speed_, direction_scalar_), delta));
        position_ = new_position;
        body_.update_hitboxes(position_);
    }
    return status_codes::nothing;
}

Vector2 entities::dog::get_direction_scalar(){
    return direction_scalar_;
}
void entities::dog::set_direction_index(size_t direction){
    if(direction < body_.num_sprites()){
        body_.set_index(direction);
    }
    if(direction < head_.num_sprites()){
        head_.set_index(direction);
    }
}
void entities::dog::determine_direction(Vector2 target){
    if(position_.x < target.x){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::right];
        set_direction_index(level_config::directions::right);
        return;
    }
    else if(position_.x > target.x){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::left];
        set_direction_index(level_config::directions::left);
        return;
    }
    else if(position_.y < target.y){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::down];
        set_direction_index(level_config::directions::down);
        return;
    }
    else if(position_.y > target.y){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::up];
        set_direction_index(level_config::directions::up);
        return;
    }
}

void entities::dog::render(Vector2 draw_position, int frame){
    entity::render(draw_position, frame);
    head_.render(draw_position, frame);
}

void entities::dog::set_path(std::vector<Vector2>& path){
    if(path.empty()) return;
    move_path_ = path;
    determine_direction(move_path_.front());
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

// ------------------------------- npc dogs ------------------------------- //
int entities::npc_dog::update(float delta, int frame){
    return dog::update(delta, frame);
}

void entities::customer_dog::entering_queue::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
}

void entities::customer_dog::waiting_in_queue::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
}

void entities::customer_dog::going_to_table::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
}

void entities::customer_dog::seated::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
}

void entities::customer_dog::eating::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
}

void entities::customer_dog::leaving::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
}

int entities::customer_dog::update(float delta, int frame){
    auto status = npc_dog::update(delta, frame);
    customer_state_->update(*this, delta, frame);
    return status;
}

void entities::customer_dog::set_state(std::unique_ptr<customer_dog::state> state){
    customer_state_ = std::move(state);
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
    // Head sprite art pending.
    // auto khiri_head_left_texture = textures::textures_.get_texture(textures::khiri_head_left, entity_config::khiri_head_left_path);
    // auto khiri_head_right_texture = textures::textures_.get_texture(textures::khiri_head_right, entity_config::khiri_head_right_path);
    // auto khiri_head_left_sprite = sprite::sprite(khiri_head_left_texture,
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::khiri_head_left_offset);
    // auto khiri_head_right_sprite = sprite::sprite(khiri_head_right_texture,
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::khiri_head_right_offset);
    // std::vector<sprite::sprite> khiri_head_sprites;
    // khiri_head_sprites.push_back(std::move(khiri_head_left_sprite));
    // khiri_head_sprites.push_back(std::move(khiri_head_right_sprite));
    // std::vector<hitbox::hitbox> khiri_head_hitboxes;
    // khiri_head_hitboxes.push_back(across_hitbox);
    // khiri_head_hitboxes.push_back(across_hitbox);
    // auto head = body::body(khiri_head_hitboxes, khiri_head_sprites);
    auto head = body::body(); // TODO fill !
    return std::make_unique<entities::player_dog>(
        std::move(body),
        std::move(head),
        std::move(khiri_outlines),
        position,
        id,
        next_debug_id("pd_"));
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
    // Head sprite art pending.
    // auto mack_head_left_texture = textures::textures_.get_texture(textures::mack_head_left, entity_config::mack_head_left_path);
    // auto mack_head_right_texture = textures::textures_.get_texture(textures::mack_head_right, entity_config::mack_head_right_path);
    // auto mack_head_left_sprite = sprite::sprite(mack_head_left_texture,
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::mack_head_left_offset);
    // auto mack_head_right_sprite = sprite::sprite(mack_head_right_texture,
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::mack_head_right_offset);
    // std::vector<sprite::sprite> mack_head_sprites;
    // mack_head_sprites.push_back(std::move(mack_head_left_sprite));
    // mack_head_sprites.push_back(std::move(mack_head_right_sprite));
    // std::vector<hitbox::hitbox> mack_head_hitboxes;
    // mack_head_hitboxes.push_back(across_hitbox);
    // mack_head_hitboxes.push_back(across_hitbox);
    // auto head = body::body(mack_head_hitboxes, mack_head_sprites);
    auto head = body::body(); // TODO fill ! and build the head sprites and hitboxes

    // and build the head, pending
    return std::make_unique<entities::player_dog>(
        std::move(body),
        std::move(head),
        std::move(mack_outlines),
        position,
        id,
        next_debug_id("pd_"));
}

// NPC dog sprite art/config pending.
// std::unique_ptr<entities::entity> entities::entity_builder::build_npc_dog(Vector2 position, int id){
//     auto npc_left_texture = textures::textures_.get_texture(textures::npc_dog_left, entity_config::npc_dog_left_path);
//     auto npc_right_texture = textures::textures_.get_texture(textures::npc_dog_right, entity_config::npc_dog_right_path);
//
//     auto npc_left_sprite = sprite::sprite(npc_left_texture,
//         entity_config::npc_dog_across_attributes[entity_config::attributes::frame_width],
//         entity_config::npc_dog_across_attributes[entity_config::attributes::frame_height],
//         entity_config::npc_dog_across_attributes[entity_config::attributes::frames],
//         entity_config::npc_dog_across_attributes[entity_config::attributes::animations]);
//     auto npc_right_sprite = sprite::sprite(npc_right_texture,
//         entity_config::npc_dog_across_attributes[entity_config::attributes::frame_width],
//         entity_config::npc_dog_across_attributes[entity_config::attributes::frame_height],
//         entity_config::npc_dog_across_attributes[entity_config::attributes::frames],
//         entity_config::npc_dog_across_attributes[entity_config::attributes::animations]);
//
//     auto across_hitbox = hitbox::h_builder_.build_player_dog_across_hitbox(position);
//     std::vector<sprite::sprite> sprites;
//     sprites.push_back(std::move(npc_left_sprite));
//     sprites.push_back(std::move(npc_right_sprite));
//     std::vector<hitbox::hitbox> hitboxes;
//     hitboxes.push_back(across_hitbox);
//     hitboxes.push_back(across_hitbox);
//     auto body = body::body(hitboxes, sprites);
//
//     // Head sprite art pending.
//     // auto npc_head_left_texture = textures::textures_.get_texture(textures::npc_dog_head_left, entity_config::npc_dog_head_left_path);
//     // auto npc_head_right_texture = textures::textures_.get_texture(textures::npc_dog_head_right, entity_config::npc_dog_head_right_path);
//     auto head = body::body();
//
//     return std::make_unique<entities::npc_dog>(
//         std::move(body),
//         std::move(head),
//         position,
//         id,
//         next_debug_id("npc_"));
// }
