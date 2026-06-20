#include "level.h"

#include "debug_log_interface.h"

namespace{
    std::string side_to_string(events::customer_queue_side queue_side){
        if(queue_side == events::customer_queue_side::left_queue){
            return "left_queue";
        }
        return "right_queue";
    }

    std::string vector_to_string(Vector2 position){
        return "{" + std::to_string(position.x) + ", " + std::to_string(position.y) + "}";
    }
}
// ----------------------------------------- level ----------------------------------------- //
void level::level::update(float delta, int frame){
    update_void_entities(delta, frame);

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

    // ---------------- debug behaviours ----------------
    auto queue_bounds = cafe_config::dog_queue_debug_bounds;
    DrawRectangleLinesEx(queue_bounds, 2.0f, RED);
    // ---------------- debug behaviours ----------------


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
    next_entity_id_ = std::max(next_entity_id_, entity_raw->get_id() + 1);
}

void level::level::add_void_entity(
    std::unique_ptr<entities::entity> entity,
    size_t layer,
    events::customer_queue_side queue_side){
    auto entity_id = entity->get_id();
    auto position = entity->get_position();
    next_entity_id_ = std::max(next_entity_id_, entity_id + 1);
    debug::log(
        "[level::add_void_entity, staged out of bounds entity] "
        "entity_id: " + std::to_string(entity_id)
        + ", queue_side: " + side_to_string(queue_side)
        + ", position: " + vector_to_string(position)
        + ", move_per_frame: " + std::to_string(cafe_config::void_entity_move_per_frame));
    void_entities_.push_back(void_entity_record{
        std::move(entity),
        layer,
        queue_side
    });
}

void level::level::update_void_entities(float delta, int frame){
    (void) delta;
    for(size_t i = 0; i < void_entities_.size();){
        auto& record = void_entities_[i];
        move_void_entity_toward_screen(*record.entity, frame);

        if(is_inside_screen(*record.entity)){
            debug::log(
                "[level::update_void_entities, entity entered screen bounds] "
                "entity_id: " + std::to_string(record.entity->get_id())
                + ", queue_side: " + side_to_string(record.queue_side)
                + ", position: " + vector_to_string(record.entity->get_position()));
            insert_void_entity(std::move(record));
            void_entities_.erase(void_entities_.begin() + static_cast<std::ptrdiff_t>(i));
            continue;
        }
        ++i;
    }
}

bool level::level::is_inside_screen(entities::entity& entity) const{
    const auto& box = entity.get_hitbox().get_box();
    return box.x >= 0.0f
        && box.y >= 0.0f
        && (box.x + box.width) <= level_config::screen_width
        && (box.y + box.height) <= level_config::screen_height;
}

void level::level::move_void_entity_toward_screen(entities::entity& entity, int frame){
    auto position = entity.get_position();
    auto previous_position = position;
    const auto& box = entity.get_hitbox().get_box();

    if(box.x < 0.0f){
        position.x += cafe_config::void_entity_move_per_frame;
    } else if((box.x + box.width) > level_config::screen_width){
        position.x -= cafe_config::void_entity_move_per_frame;
    }

    if(box.y < 0.0f){
        position.y += cafe_config::void_entity_move_per_frame;
    } else if((box.y + box.height) > level_config::screen_height){
        position.y -= cafe_config::void_entity_move_per_frame;
    }

    entity.move_without_event(position);
    debug::log(
        "[level::move_void_entity_toward_screen, moved void entity] "
        "frame: " + std::to_string(frame)
        + ", entity_id: " + std::to_string(entity.get_id())
        + ", previous_position: " + vector_to_string(previous_position)
        + ", new_position: " + vector_to_string(position)
        + ", move_per_frame: " + std::to_string(cafe_config::void_entity_move_per_frame));
}

void level::level::insert_void_entity(void_entity_record record){
    auto customer = dynamic_cast<entities::customer_dog*>(record.entity.get());
    auto entity_id = static_cast<size_t>(record.entity->get_id());
    auto queue_side = record.queue_side;
    add_entity(std::move(record.entity), record.layer);

    if(customer == nullptr){
        debug::log(
            "[level::insert_void_entity, inserted non customer entity] "
            "entity_id: " + std::to_string(entity_id));
        return;
    }

    debug::log(
        "[level::insert_void_entity, inserted customer dog] "
        "customer_id: " + std::to_string(entity_id)
        + ", queue_side: " + side_to_string(queue_side));
    auto registered_customer = events::registered_customer(entity_id);
    event_interface::execute_event(registered_customer);
    auto customer_arrived = events::customer_dog_arrived(entity_id, queue_side);
    event_interface::execute_event(customer_arrived);
}

int level::level::entity_id(){
    return next_entity_id_;
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

void level::level::on_build_dog_event(const events::build_dog& event){
    auto position = event.get_position();
    auto dog_type = event.get_dog_type();
    auto dog_id = static_cast<int>(level_entities_.get_next_id());
    debug::log(
        "[level::on_build_dog_event, received build command] "
        "dog_id: " + std::to_string(dog_id)
        + ", dog_type: " + std::to_string(dog_type)
        + ", queue_side: " + side_to_string(event.get_queue_side())
        + ", build_position: " + vector_to_string(position));
    auto new_dog = entities::e_builder.build_npc_dog(position, dog_id, dog_type);
    if(! new_dog){
        debug::log(
            "[level::on_build_dog_event, failed to build dog] "
            "dog_id: " + std::to_string(dog_id)
            + ", dog_type: " + std::to_string(dog_type));
        return;
    }
    add_void_entity(std::move(new_dog), level_config::draw_layers::dogs, event.get_queue_side());
    auto customer_id = static_cast<size_t>(dog_id);
    debug::log(
        "[level::on_build_dog_event, staged customer dog outside level bounds] "
        "customer_id: " + std::to_string(customer_id)
        + ", queue_side: " + side_to_string(event.get_queue_side())
        + ", position: " + vector_to_string(position));
}

void level::level::on_send_customer_to_queue_event(const events::send_customer_to_queue& event){
    auto customer_id = static_cast<int>(event.get_customer_id());
    debug::log(
        "[level::on_send_customer_to_queue_event, received queue path command] "
        "customer_id: " + std::to_string(customer_id)
        + ", queue_position: " + vector_to_string(event.get_queue_position()));
    auto dog_record = id_entity_map_.find(customer_id);
    if(dog_record == id_entity_map_.end()){
        debug::log(
            "[level::on_send_customer_to_queue_event, missing dog entity] "
            "customer_id: " + std::to_string(customer_id));
        return;
    }

    auto dog = static_cast<entities::dog*>(dog_record->second);
    auto direction = dog->get_direction_scalar();
    auto destination = event.get_queue_position();
    auto queue_path = graph_.find_path(dog->get_position(), destination, direction);
    debug::log(
        "[level::on_send_customer_to_queue_event, assigned queue path] "
        "customer_id: " + std::to_string(customer_id)
        + ", current_position: " + vector_to_string(dog->get_position())
        + ", destination: " + vector_to_string(destination)
        + ", path_size: " + std::to_string(queue_path.size()));
    dog->set_path(queue_path);
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
