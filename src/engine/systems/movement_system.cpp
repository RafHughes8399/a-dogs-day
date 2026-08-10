#include "component.h"
#include "entity_events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "system.h"
#include <raylib.h>

// ---------------- frame update ----------------
// TODO stub - the loop calls this every frame, nothing to do yet
void systems::movement_system::update(float delta){
    // here is where all the paths are checked [recalculated iff need be and movement occurs ]
    // i think the path recalculation is better done on a "movement entity discovers its path is blocked check" rather than a
    // decoration or some other entity is moved, as that would trigger possibly many unnecessary recalculations

    // definitely should recalc when the destination entity is moved though
    for(auto it = component_managers::movement_manager_.begin(); it != component_managers::movement_manager_.end(); ++it){
        size_t id = it->first;
        auto* movement = component_managers::movement_manager_.get_component(id);
        auto* position = component_managers::positional_manager_.get_component(id);
        if(movement == nullptr or position == nullptr){ continue; }
        if(movement->get_paths().empty()){ continue; }

        auto& current = movement->get_current_path();
        auto waypoint = current.get_next_position();

        if(movement->has_reached_position(position->get_position())){
            update_position(id, waypoint);
            current.advance();

            if(current.is_path_complete()){
                // unsure the ecs needs the same per-leg fact the entity system emitted
                // std::unique_ptr<events::event> completed = std::make_unique<events::dog_completed_path>(id, waypoint);
                // event_interface::queue_event(completed);
                movement->finish_path();
                if(not movement->get_paths().empty()){
                    determine_direction(id, *movement, position->get_position(),
                        movement->get_current_path().get_next_position());
                }
                continue;
            }
            determine_direction(id, *movement, position->get_position(), current.get_next_position());
        }

        auto step = Vector2Scale(Vector2Multiply(movement->get_move_speed(),
            movement->get_direction_scalar()), delta);
        update_position(id, Vector2Add(position->get_position(), step));
    }
}
void systems::movement_system::determine_direction(size_t id, components::movement_component& movement,
    Vector2 position, Vector2 target){
    size_t direction = level_config::directions::right;
    if(position.x < target.x){ direction = level_config::directions::right; }
    else if(position.x > target.x){ direction = level_config::directions::left; }
    else if(position.y < target.y){ direction = level_config::directions::down; }
    else if(position.y > target.y){ direction = level_config::directions::up; }
    else { return; }
    movement.set_direction_scalar(level_config::direction_scalars[direction]);
    component_helpers::set_facing_index(id, direction);
}

// ---------------- event handlers ----------------
void systems::movement_system::on_created_entity(const events::create_entity& event){
    size_t entity_id = event.get_id();
    if(component_helpers::is_mouse_positioned(entity_id)){ return; }
    if(auto* collision = component_managers::collision_manager_.get_component(entity_id); collision != nullptr){
        graph_.update_entity(collision->get_hitbox_component().get_hitbox().get_box(),
            static_cast<int>(entity_id));
    }
}
void systems::movement_system::on_moved_entity(const events::move_entity& event){
    if(component_helpers::is_mouse_positioned(event.get_id())){ return; }
    graph_.update_entity(event.get_pre_move(), graph_config::empty_node);
    graph_.update_entity(event.get_post_move(), static_cast<int>(event.get_id()));
}
void systems::movement_system::on_create_path_to_event(const events::create_path_to& event){
    auto* movement = component_managers::movement_manager_.get_component(event.get_id());
    auto* position = component_managers::positional_manager_.get_component(event.get_id());
    if(movement == nullptr or position == nullptr){ return; }

    auto mode = event.get_assignment();
    auto source = (mode == path::append and not movement->get_paths().empty())
        ? movement->get_paths().back().get_destination()
        : position->get_position();

    auto new_path = create_path(source, movement->get_direction_scalar(),
        event.get_destination(), event.get_destination_entity());
    if(not new_path.has_value()){ return; }

    switch(mode){
        case path::replace:
            movement->set_path(std::move(new_path.value()));
            determine_direction(event.get_id(), *movement, position->get_position(),
                movement->get_current_path().get_next_position());
            break;
        case path::append:
            movement->append_path(std::move(new_path.value()));
            if(movement->get_paths().size() == 1){
                determine_direction(event.get_id(), *movement, position->get_position(),
                    movement->get_current_path().get_next_position());
            }
            break;
    }
}
void systems::movement_system::on_destroyed_entity(const events::remove_entity& event){
    if(component_helpers::is_mouse_positioned(event.get_id())){ return; }
    if(auto* collision = component_managers::collision_manager_.get_component(event.get_id()); collision != nullptr){
        graph_.update_entity(collision->get_hitbox_component().get_hitbox().get_box(),
            graph_config::empty_node);
    }
}

// ---------------- path features  -----------------
std::optional<path::path> systems::movement_system::create_path(Vector2 source, Vector2 direction,
    Vector2 destination, std::optional<size_t> destination_entity){
    auto positions = graph_.find_path(source, destination, direction);
    if(positions.empty()){ return std::nullopt; }
    return path::build_path(source, destination, positions, destination_entity);
}
// ---------------- position writes ----------------
void systems::movement_system::update_position(size_t id, Vector2 new_position){
    auto* position = component_managers::positional_manager_.get_component(id);
    if(position == nullptr){ return; }
    position->set_position(new_position);

    // the hitbox is the position stored a second time, so it moves in the same
    // write - every variant, so a later facing change lands somewhere correct
    Rectangle pre_move{};
    Rectangle post_move{};
    if(auto* collision = component_managers::collision_manager_.get_component(id)){
        pre_move = collision->get_hitbox_component().get_hitbox().get_box();
        for(auto& box : collision->get_hitbox_component().get_hitboxes()){
            box.update(new_position);
        }
        post_move = collision->get_hitbox_component().get_hitbox().get_box();
    }

    // executed, not queued - the spatial index must not lag the position by a frame
    events::move_entity moved{id, pre_move, post_move};
    event_interface::execute_event(moved);
}