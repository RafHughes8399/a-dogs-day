#include "ecs_test_game.h"

#include "component.h"
#include "events.h"

namespace testing{

    namespace{
        sprite::sprite test_background(){
            return sprite::sprite(
                LoadTexture(entity_config::background_path),
                entity_config::background_attributes[entity_config::attributes::frame_width],
                entity_config::background_attributes[entity_config::attributes::frame_height],
                entity_config::background_attributes[entity_config::attributes::frames],
                entity_config::background_attributes[entity_config::attributes::animations]);
        }
    }

    ecs_test_game::ecs_test_game()
    : lifespan_(),
      rendering_(test_background(),
                 Rectangle{0.0f, 0.0f, level_config::screen_width, level_config::screen_height}){
        // one hidden window for the whole run, same as test_game - the builders
        // LoadTexture and the shared cache has to stay valid across scenarios
        if(!IsWindowReady()){
            SetConfigFlags(FLAG_WINDOW_HIDDEN);
            InitWindow(1, 1, "dog-days ecs tests");
        }
        component_helpers::clear_all_components();
    }

    ecs_test_game::~ecs_test_game(){
        events::global_dispatcher_.process_events(0.0f);
        component_helpers::clear_all_components();
        // rendering_ unsubscribes via RAII as this finishes destructing
    }

    // ---------------- build actions ----------------

    size_t ecs_test_game::create_empty(size_t layer){
        return lifespan_.create([](size_t){}, layer);
    }

    size_t ecs_test_game::create_renderable(size_t layer, Vector2 position){
        return lifespan_.create([position](size_t id){
            component_helpers::register_positional_component(id,
                component_builders::build_positional_component(position, Vector2{1.0f, 0.0f}));
            component_helpers::register_renderable_component(id,
                components::renderable_component());
        }, layer);
    }

    size_t ecs_test_game::create_player(size_t cursor_id){
        return lifespan_.create([cursor_id](size_t id){
            ecs_entities::build_player(id, cursor_id);
        }, level_config::draw_layers::hud);
    }

    void ecs_test_game::remove(size_t entity_id){
        lifespan_.remove(entity_id);
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

    size_t ecs_test_game::total_components(){
        return component_managers::positional_manager_.size()
             + component_managers::movment_manager_.size()
             + component_managers::renderable_manager_.size()
             + component_managers::collision_manager_.size()
             + component_managers::interaction_manager_.size()
             + component_managers::control_manager_.size()
             + component_managers::state_machine_manager_.size()
             + component_managers::food_manager_.size();
    }

} // namespace testing
