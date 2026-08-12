#include "ecs_test_game.h"

#include "component.h"
#include "events.h"

namespace testing{

    ecs_test_game::ecs_test_game()
    : lifespan_(systems::entity_lifespan_system::get_instance()),
      rendering_(systems::rendering_system::get_instance()),
      spatial_(systems::spatial_system::get_instance()),
      movement_(systems::movement_system::get_instance()){
        // one hidden window for the whole run, same as test_game - the builders
        // LoadTexture and the shared cache has to stay valid across scenarios
        if(not IsWindowReady()){
            SetConfigFlags(FLAG_WINDOW_HIDDEN);
            InitWindow(1, 1, "dog-days ecs tests");
        }
        component_helpers::clear_all_components();
        systems::clear_all_systems();
    }

    ecs_test_game::~ecs_test_game(){
        events::global_dispatcher_.process_events(0.0f);
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
            ecs_entities::build_waiter_dog(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_test_decoration(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_test_decoration(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_table(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_table(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_food_counter(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_counter(id, position);
        }, layer);
    }

    size_t ecs_test_game::create_dishwasher(Vector2 position, size_t layer){
        return lifespan_.create([position](size_t id){
            ecs_entities::build_dishwasher(id, position);
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
    }

    void ecs_test_game::remove(size_t entity_id){
        lifespan_.remove(entity_id);
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

    bool ecs_test_game::has_selectable(size_t entity_id){
        return component_managers::selectable_manager_.get_component(entity_id) != nullptr;
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

    int ecs_test_game::graph_occupant_at(Vector2 position){
        return movement_.graph_occupant_at(position);
    }

    size_t ecs_test_game::graph_occupied_node_count(){
        return movement_.graph_occupied_node_count();
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
             + component_managers::food_manager_.size()
             + component_managers::selectable_manager_.size();
    }

} // namespace testing
