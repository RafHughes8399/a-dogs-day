#include "level.h"
// ----------------------------------------- level ----------------------------------------- //
void level::level::update(float delta, int frame){
    std::vector<std::unique_ptr<entities::entity>> graveyard;
    auto to_remove = level_entities_.update(delta, frame, graveyard);

    if(! to_remove.empty()){
        for(auto & layer : render_layers_){
            layer.remove_entities(to_remove);
        }
    }
    // graveyard destroyed here -- dead entities stay alive until render layers are cleaned up
}
void level::level::render(int frame){
    // draw the background 
    DrawTextureRec(background_.get_texture(), view_frame_, Vector2{0.0f, 0.0f}, WHITE);
    // for debugging purposes
    // graph_.render(view_frame_);


    // draw the entities, based on the view frame
    auto render_precdicate = [this](entities::entity*& entity) -> bool { // auto is std::unique_ptr<entity>
        const Rectangle& entity_box = entity->get_hitbox().get_box();
        auto position = entity->get_position();
        (void) position;

        return view_frame_.x <= entity_box.x && view_frame_.y <= entity_box.y 
        && (view_frame_.x + view_frame_.width) >= (entity_box.x + entity_box.width) 
        && (view_frame_.y + view_frame_.height) >= (entity_box.y + entity_box.height);

    };

    for(size_t i = 0; i < level_config::draw_layers::size; ++i){
        render_layers_[i].draw(render_precdicate, Vector2{view_frame_.x, view_frame_.y}, frame);
    }
    return;
}

void level::level::add_entity(std::unique_ptr<entities::entity> entity, size_t layer){
    // make raw pointer and insert ot 
    auto entity_raw = entity.get();
    level_entities_.insert(std::move(entity));
    // insert into the draw layer ?
    render_layers_[layer].add_entity(entity_raw);
    id_entity_map_[static_cast<int>(entity_raw->get_id())] = entity_raw;
}

int level::level::entity_id(){
    return static_cast<int>(level_entities_.get_next_id());
}
int level::level::num_entities(){
    return static_cast<int>(level_entities_.size());
}

void level::level::on_left_mouse_click_event(const events::left_mouse_click& event){
    (void) event;

    return;
}
void level::level::on_move_view_frame_event(const events::move_view_frame& event){
    auto delta = event.get_delta();
    view_frame_.x = Clamp(view_frame_.x + delta.x, 0.0, level_config::world_x - GetScreenWidth());
    view_frame_.y = Clamp(view_frame_.y + delta .y, 0.0, level_config::world_y - GetScreenHeight());
}
void level::level::on_right_mouse_event(const events::right_mouse_click& event){
    auto click_position = event.get_mouse_position();
    auto paw = entities::e_builder.build_paw_mark(click_position, static_cast<int>(level_entities_.get_next_id()));
    add_entity(std::move(paw), level_config::draw_layers::hud);

    auto dog_id = event.get_selected_dog();
    if(dog_id != -1){
        auto dog = id_entity_map_[dog_id];
        auto dog_cast = static_cast<entities::player_dog*>(dog);
        auto direction = dog_cast->get_direction_scalar();
        auto dog_path = graph_.find_path(dog->get_position(), click_position, direction);
        dog_cast->set_path(dog_path);
    }
}

// --------------------- level builder ----------------------------------------- //
level::level level::level_builder::build_main_level(){
    auto background = sprite::sprite(LoadTexture(entity_config::background_path), 
        entity_config::background_attributes[entity_config::attributes::frame_width], 
        entity_config::background_attributes[entity_config::attributes::frame_height],
        entity_config::background_attributes[entity_config::attributes::frames],
        entity_config::background_attributes[entity_config::attributes::animations]);
                
    auto view_frame = Rectangle{0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    auto dimensions = Vector2{level_config::world_x, level_config::world_y};

    auto l = level(background, view_frame, dimensions);
    

    // 0 
    auto mack = entities::e_builder.build_mack(Vector2 {level_config::edge_weight * 7, level_config::edge_weight * 4}, l.entity_id());
    l.add_entity(std::move(mack), level_config::draw_layers::dogs);


    // 1
    auto khiri = entities::e_builder.build_khiri(Vector2 {level_config::edge_weight * 4, static_cast<float>(level_config::edge_weight * 3.5)}, l.entity_id());
    l.add_entity(std::move(khiri), level_config::draw_layers::dogs);

    // 2
    //  append the cursor
    auto cursor = entities::e_builder.build_cursor(GetMousePosition(), l.entity_id());
    l.add_entity(std::move(cursor), level_config::draw_layers::cursor);
    
    auto test_decoration = entities::e_builder.build_test_decoration(Vector2 {level_config::edge_weight * 6, level_config::edge_weight * 6}, l.entity_id());
    l.add_entity(std::move(test_decoration), level_config::draw_layers::decoration);
    
    auto second_decoration = entities::e_builder.build_test_decoration(Vector2 {level_config::edge_weight * 12, level_config::edge_weight * 12}, l.entity_id());
    l.add_entity(std::move(second_decoration), level_config::draw_layers::decoration);
    return l;
}
