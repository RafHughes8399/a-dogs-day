#include "level.h"
#include "debug_log_interface.h"
#include "events.h"
#include <raymath.h>
namespace{
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
    graph_.render(view_frame_);

    // ---------------- debug behaviours ----------------
    auto queue_bounds = cafe_config::queue_debug_bounds;
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
    auto table = dynamic_cast<entities::table*>(entity_raw);
    auto food_counter = dynamic_cast<entities::food_counter*>(entity_raw);
    auto waiter = dynamic_cast<entities::waiter_dog*>(entity_raw);
    auto entity_id = entity_raw->get_id();
    auto position = entity_raw->get_position();
    level_entities_.insert(std::move(entity));
    // insert into the draw layer ?
    render_layers_[layer].add_entity(entity_raw);
    id_entity_map_[static_cast<size_t>(entity_id)] = entity_raw;
    next_entity_id_ = std::max(next_entity_id_, entity_id + 1);

    if(table != nullptr){
        auto interaction_positions = table->get_interaction_positions();
        auto message = "[level::add_entity, queued table registration] "
            "table_id: " + std::to_string(entity_id)
            + ", table_position: " + vector_to_string(position)
            + ", left_interaction_position: " + vector_to_string(interaction_positions.left)
            + ", right_interaction_position: " + vector_to_string(interaction_positions.right);
        debug::log(message);
        std::unique_ptr<events::event> registered_table = std::make_unique<events::registered_table>(
            table);
        event_interface::queue_event(registered_table);
    }
    if(food_counter != nullptr){
        auto interaction_position = food_counter->get_interaction_positions().left;
        debug::log(
            "[level::add_entity, queued food counter registration] "
            "counter_id: " + std::to_string(entity_id)
            + ", position: " + vector_to_string(position)
            + ", interaction_position: " + vector_to_string(interaction_position));
        std::unique_ptr<events::event> registered_food_counter = std::make_unique<events::registered_food_counter>(
            food_counter);
        event_interface::queue_event(registered_food_counter);
    }
    if(waiter != nullptr){
        debug::log(
            "[level::add_entity, queued waiter registration] "
            "waiter_id: " + std::to_string(entity_id)
            + ", position: " + vector_to_string(position));
        std::unique_ptr<events::event> registered_waiter = std::make_unique<events::registered_waiter>(
            waiter,
            static_cast<size_t>(entity_id));
        event_interface::queue_event(registered_waiter);
    }
}

void level::level::add_void_entity(std::unique_ptr<entities::entity> entity, size_t layer){
    auto entity_id = entity->get_id();
    auto position = entity->get_position();
    next_entity_id_ = std::max(next_entity_id_, entity_id + 1);
    void_entities_.push_back(void_entity_record{
        std::move(entity),
        layer
    });
    debug::log(
        "[level::add_void_entity, added entity to void] "
        "entity_id: " + std::to_string(entity_id)
        + ", layer: " + std::to_string(layer)
        + ", position: " + vector_to_string(position)
        + ", void_entities_size: " + std::to_string(void_entities_.size()));
}

void level::level::update_void_entities(float delta, int frame){
    (void) delta;
    (void) frame;
    for(auto record = void_entities_.begin(); record != void_entities_.end();){
        move_void_entity_toward_screen(*record->entity, frame);
        if(is_inside_screen(*record->entity)){
            insert_void_entity(std::move(*record));
            record = void_entities_.erase(record);
        }
        else{
            ++record;
        }
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
    (void) frame;
    auto position = entity.get_position();
    const auto& box = entity.get_hitbox().get_box();

    // TODO, dont love this logic, clean it up
    if(box.x < 0.0f){
        position.x += level_config::void_move;
    } else if((box.x + box.width) > level_config::screen_width){
        position.x -= level_config::void_move;
    }
    if(box.y < 0.0f){
        position.y += level_config::void_move;
    } else if((box.y + box.height) > level_config::screen_height){
        position.y -= level_config::void_move;
    }

    entity.move_without_event(position);
}

void level::level::insert_void_entity(void_entity_record record){
    auto customer = dynamic_cast<entities::customer_dog*>(record.entity.get());
    auto entity_id = static_cast<size_t>(record.entity->get_id());
    auto position = record.entity->get_position();
    auto layer = record.layer;
    add_entity(std::move(record.entity), record.layer);

    if(customer == nullptr){
        debug::log(
            "[level::insert_void_entity, inserted non customer entity] "
            "entity_id: " + std::to_string(entity_id)
            + ", layer: " + std::to_string(layer)
            + ", position: " + vector_to_string(position));
        return;
    }

    debug::log(
        "[level::insert_void_entity, inserted customer entity] "
        "entity_id: " + std::to_string(entity_id)
        + ", layer: " + std::to_string(layer)
        + ", position: " + vector_to_string(position));
    
    debug::log(
        "[level::insert_void_entity, emitting customer created] "
        "entity_id: " + std::to_string(entity_id));
    auto customer_arrived = events::customer_dog_created(entity_id, position);
    event_interface::execute_event(customer_arrived);
}

int level::level::entity_id(){
    return next_entity_id_;
}

int level::level::num_entities(){
    return static_cast<int>(level_entities_.size());
}

entities::entity* level::level::get_entity(int id){
    auto entry = id_entity_map_.find(static_cast<size_t>(id));
    return entry == id_entity_map_.end() ? nullptr : entry->second;
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
    auto paw = entities::e_builder.build_paw_mark(click_position, entity_id());
    add_entity(std::move(paw), level_config::draw_layers::hud);

    auto dog_id = event.get_selected_dog();
    if(dog_id != -1){
        auto dog = id_entity_map_[static_cast<size_t>(dog_id)];
        auto dog_cast = static_cast<entities::player_dog*>(dog);
        auto direction = dog_cast->get_direction_scalar();
        auto dog_path = graph_.find_path(dog->get_position(), click_position, direction);
        dog_cast->set_path(dog_path);
    }
}
void level::level::on_build_customer_dog_event(const events::build_customer_dog& event){
    auto position = event.get_position();
    auto dog_type = event.get_dog_type();
    auto dog_id = entity_id();
    auto new_dog = entities::e_builder.build_customer_dog(dog_id, dog_type, position, std::nullopt);
    debug::log(
        "[level::on_build_customer_dog_event, built customer dog] "
        "dog_id: " + std::to_string(dog_id)
        + ", dog_type: " + std::to_string(dog_type)
        + ", spawn_position: " + vector_to_string(position));
    add_void_entity(std::move(new_dog), level_config::draw_layers::dogs);
}

void level::level::on_send_dog_to_position_event(const events::send_dog_to_position& event){
    auto customer_id = static_cast<int>(event.get_customer_id());
    auto dog_record = id_entity_map_.find(static_cast<size_t>(customer_id));
    if(dog_record == id_entity_map_.end()){
        debug::log(
            "[level::on_send_dog_to_position_event, missing customer entity] "
            "customer_id: " + std::to_string(customer_id)
            + ", destination: " + vector_to_string(event.get_destination()));
        return;
    }

    auto dog = static_cast<entities::dog*>(dog_record->second);
    auto direction = dog->get_direction_scalar();
    auto source = event.has_source() ? event.get_source() : dog->get_position();
    auto destination = event.get_destination();
    auto path = graph_.find_path(source, destination, direction);
    std::unique_ptr<events::event> give_dog_path = std::make_unique<events::give_dog_path>(customer_id, path);
    event_interface::queue_event(give_dog_path);
}

void level::level::on_send_dog_to_furniture(const events::send_dog_to_furniture& event){
    // find the dog, calculate the path, give the dog the path
    auto dog_id = static_cast<int>(event.get_dog_id());
    auto dog_record = id_entity_map_.find(static_cast<size_t>(dog_id));
    if(dog_record == id_entity_map_.end()){
        debug::log(
            "[level::on_send_dog_to_furniture, missing customer entity] "
            "customer_id: " + std::to_string(dog_id)
            + ", table_id: " + std::to_string(event.get_furniture_id())
            + ", destination: " + vector_to_string(event.get_destination()));
        return;
    }
    auto dog = static_cast<entities::dog*>(dog_record->second);
    auto position = (event.get_source() == std::nullopt) ? dog->get_position() : event.get_source().value();
    auto path = graph_.find_path(position, event.get_destination(), dog->get_direction_scalar());

    dog->set_path(path, static_cast<int>(event.get_furniture_id()), event.get_furniture_position());
}
void level::level::on_removed_entity(const events::remove_entity& event){
    size_t id = event.get_id();
    // Drop the entity from every render layer before it is destroyed below.
    // add_entity pushed its raw pointer into one of the render_layers_; leaving
    // it there would dangle once level_entities_.erase frees the entity.
    // render_layer::remove_entity matches by address, so clearing all layers is
    // safe and only the layer actually holding it is affected.
    auto entry = id_entity_map_.find(id);
    if(entry != id_entity_map_.end()){
        for(auto& layer : render_layers_){
            layer.remove_entity(entry->second);
        }
    }
    // Destroys the entity (quadtree owns it) and fires the in-place removed_*
    // events so the cafe systems drop their pointers, then clear the id lookup.
    level_entities_.erase(id);
    id_entity_map_.erase(id);
}
void level::level::on_order_served_event(const events::order_served& event){
    auto customer_id = static_cast<int>(event.get_customer_id());
    auto customer_record = id_entity_map_.find(static_cast<size_t>(customer_id));
    if(customer_record == id_entity_map_.end()){
        debug::log(
            "[level::on_order_served_event, missing customer entity] "
            "customer_id: " + std::to_string(customer_id)
            + ", order_id: " + std::to_string(event.get_order_id())
            + ", table_id: " + std::to_string(event.get_table_id()));
        return;
    }

    auto customer = dynamic_cast<entities::customer_dog*>(customer_record->second);
    if(customer == nullptr){
        debug::log(
            "[level::on_order_served_event, entity is not customer dog] "
            "customer_id: " + std::to_string(customer_id)
            + ", order_id: " + std::to_string(event.get_order_id())
            + ", table_id: " + std::to_string(event.get_table_id()));
        return;
    }

    customer->set_eating(event.get_order_id(), event.get_table_id(), event.get_table_position());
}

// --------------------- level builder ----------------------------------------- //
std::unique_ptr<level::level> level::level_builder::build_main_level(){
    auto background = sprite::sprite(LoadTexture(entity_config::background_path), 
        entity_config::background_attributes[entity_config::attributes::frame_width], 
        entity_config::background_attributes[entity_config::attributes::frame_height],
        entity_config::background_attributes[entity_config::attributes::frames],
        entity_config::background_attributes[entity_config::attributes::animations]);
                
    auto view_frame = Rectangle{0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    auto dimensions = Vector2{level_config::world_x, level_config::world_y};

    auto main_level = std::make_unique<level>(background, view_frame, dimensions);
    

    // 0 
    auto mack = entities::e_builder.build_mack(Vector2 {level_config::edge_weight * 7, level_config::edge_weight * 4}, main_level->entity_id());
    main_level->add_entity(std::move(mack), level_config::draw_layers::dogs);


    // 1
    auto khiri = entities::e_builder.build_khiri(Vector2 {level_config::edge_weight * 4, static_cast<float>(level_config::edge_weight * 3.5)}, main_level->entity_id());
    main_level->add_entity(std::move(khiri), level_config::draw_layers::dogs);

    // 2
    //  append the cursor
    auto cursor = entities::e_builder.build_cursor(GetMousePosition(), main_level->entity_id());
    main_level->add_entity(std::move(cursor), level_config::draw_layers::cursor);
    
    auto first_table = entities::e_builder.build_table(Vector2 {level_config::edge_weight * 6, level_config::edge_weight * 6}, main_level->entity_id());
    main_level->add_entity(std::move(first_table), level_config::draw_layers::stations);
    
    auto second_table = entities::e_builder.build_table(Vector2 {level_config::edge_weight * 12, level_config::edge_weight * 12}, main_level->entity_id());
    main_level->add_entity(std::move(second_table), level_config::draw_layers::stations);

    auto food_counter = entities::e_builder.build_food_counter(Vector2 {level_config::edge_weight * 18, level_config::edge_weight * 6}, main_level->entity_id());
    // Pre-stock the counter with food. Nothing produces food yet, so this stands
    // in for a cook/kitchen until one is built; it lets the expediter model real
    // counter availability (is_empty()/take()) for the serving flow.
    if(auto* counter = dynamic_cast<entities::food_counter*>(food_counter.get())){
        for(size_t i = 0; i < counter->max_capacity(); ++i){
            counter->store(entities::e_builder.build_test_food(counter->get_position(), main_level->entity_id()));
        }
    }
    main_level->add_entity(std::move(food_counter), level_config::draw_layers::stations);

    auto waiter = entities::e_builder.build_waiter_dog(main_level->entity_id(), dog_config::waiter_dog_types::basic, {level_config::edge_weight * 22, level_config::edge_weight * 7}, std::nullopt);
    main_level->add_entity(std::move(waiter), level_config::draw_layers::dogs);

    return main_level;
}
