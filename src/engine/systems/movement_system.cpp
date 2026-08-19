#include "component.h"
#include "debug_log_interface.h"
#include "entity_events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "raglib.h"
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
        auto box = collision->get_hitbox_component().get_hitbox().get_box();
        for(auto* graph : graphs()){
            graph->update_entity(box, static_cast<int>(entity_id));
        }
    }
}
void systems::movement_system::on_moved_entity(const events::move_entity& event){
    if(component_helpers::is_mouse_positioned(event.get_id())){ return; }
    for(auto* graph : graphs()){
        graph->remove_entity(event.get_pre_move(), static_cast<int>(event.get_id()));
        graph->update_entity(event.get_post_move(), static_cast<int>(event.get_id()));
    }
}
void systems::movement_system::on_create_path_to_event(const events::create_path_to& event){
    auto* movement = component_managers::movement_manager_.get_component(event.get_id());
    auto* position = component_managers::positional_manager_.get_component(event.get_id());
    if(movement == nullptr or position == nullptr){ return; }

    auto mode = event.get_assignment();
    auto source = (mode == path::append and not movement->get_paths().empty())
        ? movement->get_paths().back().get_destination()
        : position->get_position();
    auto destination = event.get_destination();
    auto& source_graph = resolve_graph(source);
    auto& destination_graph = resolve_graph(destination);
    if(source_graph != destination_graph) {return;}

    debug::log("[movement_system::on_create_path_to_event] dog: " + std::to_string(event.get_id())
        + ", source: " + raglib::vector_to_string(source)
        + ", event destination (click position): " + raglib::vector_to_string(destination)
        + ", destination_entity: " + (event.get_destination_entity().has_value()
            ? std::to_string(event.get_destination_entity().value()) : std::string("none")));
    if(event.get_destination_entity().has_value()){
        auto destination_entity_id = event.get_destination_entity().value();
        auto interactable = component_managers::interactable_manager_.get_component(destination_entity_id);
        if(interactable){
            auto* destination_position = component_managers::positional_manager_.get_component(destination_entity_id);
            debug::log("[movement_system::on_create_path_to_event] destination entity actual position: "
                + (destination_position != nullptr
                    ? raglib::vector_to_string(destination_position->get_position())
                    : std::string("no position component")));
            if(destination_position != nullptr){
                destination = destination_position->get_position();
                auto interaction_offset = interactable->get_interaction_offset(source, destination);
                if(interaction_offset.has_value()){
                    destination = Vector2Add(destination, interaction_offset.value());
                    if(auto* slot_node = destination_graph.node_at(destination); slot_node != nullptr and destination_graph.position_in_area(slot_node->position_)){
                        destination = slot_node->position_;
                    }
                }
            }
        }
    }
    debug::log("[movement_system::on_create_path_to_event] final destination used for pathing: "
        + raglib::vector_to_string(destination));
    auto new_path = create_path(source, movement->get_direction_scalar(),
        destination, event.get_destination_entity());
    if(not new_path.has_value()){
        debug::log("[movement_system::on_create_path_to_event] create_path FAILED - no path found from "
            + raglib::vector_to_string(source) + " to " + raglib::vector_to_string(destination));
        return;
    }
    debug::log("[movement_system::on_create_path_to_event] create_path SUCCEEDED, first waypoint: "
        + raglib::vector_to_string(new_path.value().get_next_position()));

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
        auto box = collision->get_hitbox_component().get_hitbox().get_box();
        for(auto* graph : graphs()){
            graph->remove_entity(box, static_cast<int>(event.get_id()));
        }
    }
}

// ---------------- path features  -----------------
std::optional<path::path> systems::movement_system::create_path(Vector2 source, Vector2 direction,
    Vector2 destination, std::optional<size_t> destination_entity){
    auto& source_graph = resolve_graph(source);
    auto& destination_graph = resolve_graph(destination);
    if(source_graph != destination_graph) {return std::nullopt;}
    auto positions = source_graph.find_path(source, destination, direction);
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

// ---------------- graph resolver ----------------
graph::level_graph& systems::movement_system::resolve_graph(Vector2 position){
    return cafe_.position_in_area(position) ? cafe_ : footpath_;
}
std::array<graph::level_graph*, 2> systems::movement_system::graphs(){
    return {&cafe_, &footpath_};
}
