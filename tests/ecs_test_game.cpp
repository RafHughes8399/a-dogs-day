#include <catch2/catch_test_macros.hpp>

#include "ecs_test_game.h"

#include "component.h"
#include "events.h"
#include "events_interface.h"
#include "system_events.h"

namespace testing{

    ecs_test_game::ecs_test_game()
    : lifespan_(systems::entity_lifespan_system::get_instance()),
      rendering_(systems::rendering_system::get_instance()),
      spatial_(systems::spatial_system::get_instance()),
      movement_(systems::movement_system::get_instance()),
      interaction_(systems::interaction_system::get_instance()),
      animation_(systems::animation_system::get_instance()){
        // one hidden window for the whole run, same as test_game - the builders
        // LoadTexture and the shared cache has to stay valid across scenarios
        if(not IsWindowReady()){
            SetConfigFlags(FLAG_WINDOW_HIDDEN);
            InitWindow(1, 1, "dog-days ecs tests");
        }
        component_helpers::clear_all_components();
        systems::clear_all_systems();
        interaction_.restore_interaction_behaviours();
    }

    ecs_test_game::~ecs_test_game(){
        events::global_dispatcher_.process_events(0.0f);
        interaction_.restore_interaction_behaviours();
        component_helpers::clear_all_components();
        systems::clear_all_systems();
    }

    // ---------------- build actions ----------------

    size_t ecs_test_game::create_empty(size_t layer){
        return lifespan_.create([](size_t){}, layer);
    }

    size_t ecs_test_game::create_renderable(size_t layer, Vector2 position){
        return lifespan_.create([position](size_t id){
            component_helpers::register_positional_component(id,
                component_builders::build_positional_component(position));
            component_helpers::register_renderable_component(id,
                components::renderable_component());
        }, layer);
    }

    size_t ecs_test_game::create_player(size_t cursor_id){
        return lifespan_.create([cursor_id](size_t id){
            ecs_entities::build_player(id, cursor_id);
        }, level_config::draw_layers::hud);
    }

    size_t ecs_test_game::create_cursor(size_t layer){
        return lifespan_.create([](size_t id){
            ecs_entities::build_cursor(id);
        }, layer);
    }

    size_t ecs_test_game::create_khiri(size_t layer){
        return lifespan_.create([](size_t id){
            ecs_entities::build_khiri(id);
        }, layer);
    }

    size_t ecs_test_game::create_mack(size_t layer){
        return lifespan_.create([](size_t id){
            ecs_entities::build_mack(id);
        }, layer);
    }

    size_t ecs_test_game::create_customer_dog(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_customer_dog(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_waiter_dog(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_gianluca(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_test_decoration(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_test_decoration(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_table(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_dining_table(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_food_counter(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_food_counter(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_dishwasher(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_dishwasher(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_stove(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_stove(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_food(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_food(id, position);
        }, layer);
    }

    void ecs_test_game::tick(float delta){
        events::global_dispatcher_.process_events(delta);
        movement_.update(delta);
        interaction_.update(delta);
        animation_.update(delta);
    }

    bool ecs_test_game::tick_until(std::function<bool()> predicate, int max_frames, float delta){
        for(int frame = 0; frame < max_frames; ++frame){
            if(predicate()){ return true; }
            tick(delta);
        }
        return predicate();
    }

    void ecs_test_game::path_to(size_t entity_id, Vector2 destination,
        std::optional<size_t> destination_entity, std::vector<Vector2> checkpoints){
        std::unique_ptr<events::event> request;
        if(destination_entity.has_value()){
            request = std::make_unique<events::create_path_to_entity>(entity_id,
                destination_entity.value(), path::replace, std::move(checkpoints));
        }
        else{
            request = std::make_unique<events::create_path_to>(entity_id, destination,
                path::replace, std::move(checkpoints));
        }
        event_interface::queue_event(request);
        tick(0.0f);
    }

    size_t ecs_test_game::queued_path_count(size_t entity_id){
        auto* movement = component_managers::movement_manager_.get_component(entity_id);
        return movement == nullptr ? 0 : movement->get_paths().size();
    }

    std::vector<Vector2> ecs_test_game::path_destinations(size_t entity_id){
        std::vector<Vector2> destinations;
        auto* movement = component_managers::movement_manager_.get_component(entity_id);
        if(movement == nullptr){ return destinations; }
        auto paths = movement->get_paths();
        while(not paths.empty()){
            destinations.push_back(paths.front().get_destination());
            paths.pop();
        }
        return destinations;
    }

    void ecs_test_game::remove(size_t entity_id){
        lifespan_.destroy(entity_id);
    }

    void ecs_test_game::move_entity(size_t entity_id, Vector2 position){
        movement_.update_position(entity_id, position);
    }

    // ---------------- inspection accessors ----------------

    size_t ecs_test_game::layer_size(size_t layer){
        return rendering_.get_layer(layer).size();
    }

    bool ecs_test_game::layer_contains(size_t layer, size_t entity_id){
        return rendering_.get_layer(layer).contains(entity_id);
    }

    Rectangle ecs_test_game::view_frame(){
        return rendering_.get_view_frame();
    }

    size_t ecs_test_game::num_components(size_t entity_id){
        return component_helpers::num_registered_components(entity_id);
    }

    bool ecs_test_game::has_position(size_t entity_id){
        return component_managers::positional_manager_.get_component(entity_id) != nullptr;
    }

    bool ecs_test_game::has_renderable(size_t entity_id){
        return component_managers::renderable_manager_.get_component(entity_id) != nullptr;
    }

    bool ecs_test_game::has_controls(size_t entity_id){
        return component_managers::control_manager_.get_component(entity_id) != nullptr;
    }

    bool ecs_test_game::has_collision(size_t entity_id){
        return component_managers::collision_manager_.get_component(entity_id) != nullptr;
    }

    bool ecs_test_game::has_mouse_input(size_t entity_id){
        return component_managers::mouse_input_manager_.get_component(entity_id) != nullptr;
    }

    bool ecs_test_game::has_movement(size_t entity_id){
        return component_managers::movement_manager_.get_component(entity_id) != nullptr;
    }

    bool ecs_test_game::has_path(size_t entity_id){
        auto* movement = component_managers::movement_manager_.get_component(entity_id);
        return movement != nullptr and not movement->get_paths().empty();
    }

    float ecs_test_game::facing_x_of(size_t entity_id){
        return component_managers::movement_manager_.get_component(entity_id)
            ->get_direction_scalar().x;
    }

    bool ecs_test_game::has_selectable(size_t entity_id){
        return component_managers::selectable_manager_.get_component(entity_id) != nullptr;
    }
    bool ecs_test_game::has_interactor(size_t entity_id){
        return component_managers::interactor_manager_.get_component(entity_id) != nullptr;
    }
    bool ecs_test_game::has_storage(size_t entity_id){
        return component_managers::storage_manager_.get_component(entity_id) != nullptr;
    }
    bool ecs_test_game::has_interactable(size_t entity_id){
        return component_managers::interactable_manager_.get_component(entity_id) != nullptr;
    }

    size_t ecs_test_game::selectable_kind_of(size_t entity_id){
        return component_managers::selectable_manager_.get_component(entity_id)->get_kind();
    }

    bool ecs_test_game::is_tracked(size_t entity_id){
        return spatial_.is_tracked(entity_id);
    }

    size_t ecs_test_game::tracked_count(){
        return spatial_.tracked_count();
    }

    int ecs_test_game::node_depth_of(size_t entity_id){
        return spatial_.node_depth_of(entity_id);
    }

    bool ecs_test_game::node_bounds_of(size_t entity_id, raglib::bounding_box_2& bounds){
        return spatial_.node_bounds_of(entity_id, bounds);
    }

    Rectangle ecs_test_game::hitbox_of(size_t entity_id){
        auto* collision = component_managers::collision_manager_.get_component(entity_id);
        return collision->get_hitbox_component().get_hitbox().get_box();
    }

    size_t ecs_test_game::in_flight_animation_count(){
        return animation_.in_flight_count();
    }

    size_t ecs_test_game::interaction_count(){
        return interaction_.interaction_count();
    }

    bool ecs_test_game::has_interaction(size_t interactor, size_t interactee){
        return interaction_.has_interaction(interactor, interactee);
    }

    std::vector<size_t> ecs_test_game::performable_interactions_of(size_t interactor, size_t interactee){
        return interaction_.performable_interactions_of(interactor, interactee);
    }

    void ecs_test_game::set_interaction_behaviour(size_t index,
        std::function<void(size_t, size_t, float)> behaviour){
        interaction_.set_interaction_behaviour(index, std::move(behaviour));
    }

    void ecs_test_game::restore_interaction_behaviours(){
        interaction_.restore_interaction_behaviours();
    }

    void ecs_test_game::claim(size_t interactor, size_t interactee){
        auto* interactable = component_managers::interactable_manager_.get_component(interactee);
        auto* actor = component_managers::interactor_manager_.get_component(interactor);
        REQUIRE(interactable != nullptr);
        REQUIRE(actor != nullptr);
        REQUIRE(interactable->claim(interactor));
        actor->interact_with(interactee);
    }

    void ecs_test_game::unclaim(size_t interactor, size_t interactee){
        auto* interactable = component_managers::interactable_manager_.get_component(interactee);
        auto* actor = component_managers::interactor_manager_.get_component(interactor);
        if(interactable != nullptr){ interactable->release(interactor); }
        if(actor != nullptr){ actor->stop_interacting(); }
    }

    int ecs_test_game::graph_occupant_at(Vector2 position){
        return movement_.graph_occupant_at(position);
    }

    size_t ecs_test_game::graph_occupied_node_count(){
        return movement_.graph_occupied_node_count();
    }

    int ecs_test_game::graph_cell_at_index(Vector2 position){
        return movement_.graph_cell_at_index(position);
    }

    int ecs_test_game::graph_nearest_node_index(Vector2 position){
        return movement_.graph_nearest_node_index(position);
    }

    Vector2 ecs_test_game::graph_node_position_at(Vector2 position){
        auto* node = movement_.node_at(position);
        return node == nullptr ? Vector2{-1.0f, -1.0f} : node->position_;
    }

    bool ecs_test_game::graph_marks(size_t entity_id, Rectangle footprint){
        for(auto col = footprint.x; col < footprint.x + footprint.width; col += level_config::edge_weight){
            for(auto row = footprint.y; row < footprint.y + footprint.height; row += level_config::edge_weight){
                if(graph_occupant_at(Vector2{col, row}) != static_cast<int>(entity_id)){
                    return false;
                }
            }
        }
        return true;
    }

    size_t ecs_test_game::total_components(){
        return component_managers::positional_manager_.size()
             + component_managers::movement_manager_.size()
             + component_managers::renderable_manager_.size()
             + component_managers::collision_manager_.size()
             + component_managers::interactor_manager_.size()
             + component_managers::interactable_manager_.size()
             + component_managers::control_manager_.size()
             + component_managers::mouse_input_manager_.size()
             + component_managers::state_machine_manager_.size()
             + component_managers::selectable_manager_.size()
             + component_managers::storage_manager_.size();
    }

} // namespace testing
