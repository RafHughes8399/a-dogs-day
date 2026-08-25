#include "component.h"
#include "debug_log_interface.h"
#include "entity_events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "raglib.h"
#include "system.h"
#include <algorithm>
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
    create_path_to(event.get_id(), event.get_destination(), event.get_checkpoints(),
        event.get_assignment());
}
void systems::movement_system::on_create_path_to_entity_event(const events::create_path_to_entity& event){
    create_path_to_entity(event.get_id(), event.get_destination_entity(),
        event.get_checkpoints(), event.get_assignment());
}

void systems::movement_system::create_path_to(size_t entity_id, Vector2 destination,
    const std::vector<Vector2>& checkpoints, path::assignment mode){
    auto* movement = component_managers::movement_manager_.get_component(entity_id);
    auto* position = component_managers::positional_manager_.get_component(entity_id);
    if(movement == nullptr or position == nullptr){ return; }

    auto source = (mode == path::append and not movement->get_paths().empty())
        ? movement->get_paths().back().get_destination()
        : position->get_position();

    debug::log("[movement_system::create_path_to] dog: " + std::to_string(entity_id)
        + ", source: " + raglib::vector_to_string(source)
        + ", destination: " + raglib::vector_to_string(destination)
        + ", checkpoints: " + std::to_string(checkpoints.size()));

    // * every leg is built before any of it is committed - a route that failed
    // * halfway would strand the dog at a checkpoint with no way on
    std::vector<path::path> legs;
    if(not build_legs(source, movement->get_direction_scalar(), destination,
        std::nullopt, checkpoints, legs)){
        debug::log("[movement_system::create_path_to] route FAILED from "
            + raglib::vector_to_string(source) + " to "
            + raglib::vector_to_string(destination) + " - whole route abandoned");
        return;
    }
    commit_route(entity_id, *movement, *position, mode, std::move(legs));
}

void systems::movement_system::create_path_to_entity(size_t entity_id, size_t destination_entity,
    const std::vector<Vector2>& checkpoints, path::assignment mode){
    auto* movement = component_managers::movement_manager_.get_component(entity_id);
    auto* position = component_managers::positional_manager_.get_component(entity_id);
    if(movement == nullptr or position == nullptr){ return; }

    auto source = (mode == path::append and not movement->get_paths().empty())
        ? movement->get_paths().back().get_destination()
        : position->get_position();

    auto* destination_position = component_managers::positional_manager_.get_component(destination_entity);
    if(destination_position == nullptr){
        debug::log("[movement_system::create_path_to_entity] destination entity has no position, entity: "
            + std::to_string(destination_entity) + " - route abandoned");
        return;
    }
    auto destination = destination_position->get_position();

    // * the interaction offset moves the destination to a free slot around the
    // * entity, not onto the entity itself - a dog pathing to the table's own
    // * position would be pathing into an occupied, unwalkable node
    auto interactable = component_managers::interactable_manager_.get_component(destination_entity);
    if(interactable){
        auto interaction_offset = interactable->get_interaction_offset(source, destination);
        if(interaction_offset.has_value()){
            destination = Vector2Add(destination, interaction_offset.value());
            auto& destination_graph = resolve_graph(destination);
            if(auto* slot_node = destination_graph.node_at(destination);
                slot_node != nullptr and destination_graph.position_in_area(slot_node->position_)){
                destination = slot_node->position_;
            }
        }
    }
    debug::log("[movement_system::create_path_to_entity] dog: " + std::to_string(entity_id)
        + ", source: " + raglib::vector_to_string(source)
        + ", destination entity: " + std::to_string(destination_entity)
        + ", resolved destination: " + raglib::vector_to_string(destination)
        + ", checkpoints: " + std::to_string(checkpoints.size()));

    std::vector<path::path> legs;
    if(not build_legs(source, movement->get_direction_scalar(), destination,
        destination_entity, checkpoints, legs)){
        debug::log("[movement_system::create_path_to_entity] route FAILED from "
            + raglib::vector_to_string(source) + " to "
            + raglib::vector_to_string(destination) + " - whole route abandoned");
        return;
    }
    commit_route(entity_id, *movement, *position, mode, std::move(legs));
}

void systems::movement_system::commit_route(size_t entity_id, components::movement_component& movement,
    components::position_component& position, path::assignment mode, std::vector<path::path> legs){
    // * the mode applies to the route, not to each leg - legs after the first
    // * always append, or each would wipe the one before it
    if(mode == path::replace){ movement.clear_paths(); }
    const bool start_from_idle = movement.get_paths().empty();
    for(auto& leg : legs){ movement.append_path(std::move(leg)); }
    if(start_from_idle){
        determine_direction(entity_id, movement, position.get_position(),
            movement.get_current_path().get_next_position());
    }
}

bool systems::movement_system::build_legs(Vector2 source, Vector2 direction, Vector2 destination,
    std::optional<size_t> destination_entity, const std::vector<Vector2>& checkpoints,
    std::vector<path::path>& legs){
    auto leg_source = source;
    for(auto checkpoint : checkpoints){
        if(not build_leg(leg_source, direction, checkpoint, std::nullopt, legs)){
            debug::log("[movement_system::build_legs] checkpoint leg FAILED from "
                + raglib::vector_to_string(leg_source) + " to "
                + raglib::vector_to_string(checkpoint));
            return false;
        }
        leg_source = checkpoint;
    }
    // * only the leg left after the checkpoints run out carries the destination
    // * entity - it is what tells arrival which entity was reached, and a
    // * checkpoint reaches nothing
    return build_leg(leg_source, direction, destination, destination_entity, legs);
}

bool systems::movement_system::build_leg(Vector2 source, Vector2 direction, Vector2 destination,
    std::optional<size_t> destination_entity, std::vector<path::path>& legs){
    // * whichever graph holds both ends plans the leg. asking resolve_graph
    // * would not do - it always answers cafe for a position in the overlap
    // * band, so a leg leaving the band for the footpath could never be planned
    for(auto* candidate : graphs()){
        if(not candidate->position_in_area(source)){ continue; }
        if(not candidate->position_in_area(destination)){ continue; }
        auto leg = create_path(*candidate, source, direction, destination, destination_entity);
        if(not leg.has_value()){ continue; }
        legs.push_back(std::move(leg.value()));
        return true;
    }
    return false;
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
std::optional<path::path> systems::movement_system::create_path(graph::level_graph& graph,
    Vector2 source, Vector2 direction, Vector2 destination,
    std::optional<size_t> destination_entity){
    if(not graph.position_in_area(source)){ return std::nullopt; }
    if(not graph.position_in_area(destination)){ return std::nullopt; }
    auto positions = graph.find_path(source, destination, direction);
    if(positions.empty()){ return std::nullopt; }
    return path::build_path(source, destination, positions, destination_entity);
}
std::optional<path::path> systems::movement_system::create_path(Vector2 source, Vector2 direction,
    Vector2 destination, std::optional<size_t> destination_entity){
    auto& source_graph = resolve_graph(source);
    if(not source_graph.position_in_area(source)){ return std::nullopt; }
    return create_path(source_graph, source, direction, destination, destination_entity);
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
