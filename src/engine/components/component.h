#ifndef COMPONENT_H
#define COMPONENT_H

#include <concepts>
#include <queue>
#include <stddef.h>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config.h"
#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "raylib.h"
#include "raymath.h"
#include "sprite.h"
namespace components {

// * components carry no id of their own - the manager's map key IS the owning
// * entity, so anything iterating components already has it.
class position_component {
public:
    ~position_component() = default;
    // TODO set default value for direction scalar 
    position_component(Vector2 position, Vector2 direction_scalar)
        : position_(position), direction_scalar_(direction_scalar) {}
    position_component(const position_component& other) = default;
    position_component(position_component&& other) = default;

    position_component& operator=(const position_component& other) = default;
    position_component& operator=(position_component&& other) = default;

    Vector2 get_position();
    Vector2 get_direction_scalar();
private:
    Vector2 position_;
    Vector2 direction_scalar_;
};
class movement_component {

public:
    ~movement_component() = default;
    movement_component(Vector2 move_speed, std::queue<type_config::path> paths = {})
        : paths_(paths), move_speed_(move_speed) {}
    movement_component(const movement_component& other) = default;
    movement_component(movement_component&& other) = default;

    movement_component& operator=(const movement_component& other) = default;
    movement_component& operator=(movement_component&& other) = default;

    type_config::path get_current_path();
    Vector2 get_move_speed();
private:
    std::queue<type_config::path> paths_;
    // * not const - a const member deletes both assignment operators, which
    // * the manager needs to store components by value.
    Vector2 move_speed_;
};

// * an entity gets ONE renderable_component holding however many sprites it
// * needs - body, outlines and cosmetics all sit in the same list. The
// * multiplicity lives here rather than in the manager, so the manager stays
// * one entity to one component.
class renderable_component {
public:
  class sprite_component {
  public:
    ~sprite_component() = default;
    // TODO fix magic number what is 0
    sprite_component(std::vector<sprite::sprite> &sprites, size_t index = 0)
        : sprites_(sprites), sprite_index_(index) {}
    sprite_component(const sprite_component& other) = default;
    sprite_component(sprite_component&& other) = default;

    sprite_component& operator=(const sprite_component& other) = default;
    sprite_component& operator=(sprite_component&& other) = default;
    
      sprite::sprite& get_sprite();
      size_t get_sprite_index() const;
      void set_index(size_t index);

  private:
    std::vector<sprite::sprite> sprites_;
    size_t sprite_index_;
  };

  ~renderable_component() = default;
  renderable_component(std::vector<sprite_component> sprites = {})
      : sprites_(std::move(sprites)) {}
  renderable_component(const renderable_component& other) = default;
  renderable_component(renderable_component&& other) = default;

  renderable_component& operator=(const renderable_component& other) = default;
  renderable_component& operator=(renderable_component&& other) = default;

  std::vector<sprite_component>& get_sprites();
private:
  std::vector<sprite_component> sprites_;

};
// TODO at a later point, we aren't up to collision and interaction yet even
// currently
class collision_component {
  // defines collision behaviour
public:
  ~collision_component() = default;
  collision_component() = default;
  collision_component(const collision_component& other) = default;
  collision_component(collision_component&& other) = default;

  collision_component& operator=(const collision_component& other) = default;
  collision_component& operator=(collision_component&& other) = default;
};


// for stations to allow for interactions with
class interaction_component {
  // supports interaction
public:
  ~interaction_component() = default;
  interaction_component() = default;
  interaction_component(const interaction_component& other) = default;
  interaction_component(interaction_component&& other) = default;

  interaction_component& operator=(const interaction_component& other) = default;
  interaction_component& operator=(interaction_component&& other) = default;
};

// ! A STATE MACHINE IS A SET OF STATE COMPONENTS
// ! a STATE MACHINE COMPONENT IS A SET OF STATE COMPONENTS
// A STATE MACHINE DEFFINES TRANSITIONS BETWEEN STATES 
    // THE CONDITION TO TRANISITION
    // THE BEHAVIOUR OF TRANSITIONING
    // AND THE NEXT STATE
// bruh we dont 
class state_machine_component{
public:
    // * virtual dtor because this is a polymorphic base - concrete states
    // * derive from it and are held through a base pointer.
    class state_component {
    public:
        virtual ~state_component() = default;
        state_component() = default;
        state_component(const state_component& other) = default;
        state_component(state_component&& other) = default;

        state_component& operator=(const state_component& other) = default;
        state_component& operator=(state_component&& other) = default;
    };

    ~state_machine_component() = default;
    state_machine_component() = default;
    state_machine_component(const state_machine_component& other) = default;
    state_machine_component(state_machine_component&& other) = default;

    state_machine_component& operator=(const state_machine_component& other) = default;
    state_machine_component& operator=(state_machine_component&& other) = default;

    //std::vector<state_component>
};
// for the table, the waiter, and the counter and the kitchen station
class food_component {
  // ? current idea for the storage component is for stations to store food,
  // ? tables to store foood
  // ? and dishwasher to store plates
  public:
    ~food_component() = default;
    food_component() = default;
    food_component(const food_component& other) = default;
    food_component(food_component&& other) = default;

    food_component& operator=(const food_component& other) = default;
    food_component& operator=(food_component&& other) = default;
  private:
};

// menu component ?
// hud component ? 

} // namespace components

namespace managers {
    template <typename C> // C for component
class component_manager {
public:
   ~component_manager() = default;
   component_manager() = default;
   component_manager(const component_manager& other) = default;
   component_manager(component_manager&& other) = default;

   component_manager& operator=(const component_manager& other) = default;
   component_manager& operator=(component_manager&& other) = default;

   void register_component(size_t entity, C component){
        components_[entity] = component;
   }
   // * only need to unregister components when the entity is removed from the game
   void unregister_component(size_t entity){
        components_.erase(entity);
   }
   // get component for an entity
   C* get_component(size_t entity){
        auto component_it = components_.find(entity);
        return component_it != components_.end() ? &component_it->second : nullptr;
   }
   
private:
    // * one entity to one component. Where an entity needs several of something
    // * (sprites, states) the component itself holds the list - see
    // * renderable_component and state_machine_component.
    std::unordered_map<size_t, C> components_;
};

// * one manager instance per component type, defined in component_managers.cpp.
// * these live in their own namespace so call sites read
// * managers::renderable_manager_ rather than components::renderable_manager_,
// * which would blur the storage layer into the data layer.
extern component_manager<components::position_component> positional_manager_;
extern component_manager<components::movement_component> movment_manager_;
extern component_manager<components::renderable_component> renderable_manager_;
extern component_manager<components::collision_component> collision_manager_;
extern component_manager<components::interaction_component> interaction_manager_;
extern component_manager<components::state_machine_component> state_machine_manager_;
extern component_manager<components::food_component> food_manager_;
} // namespace managers

namespace builders{
    components::position_component build_positional_component(Vector2 position, Vector2 direction_scalar);
    components::movement_component build_movement_component(Vector2 move_speed, std::queue<type_config::path> paths = {});
    components::renderable_component::sprite_component build_sprite_component(std::vector<sprite::sprite>& sprites, size_t index);
    components::renderable_component build_renderable_component(
        std::vector<components::renderable_component::sprite_component>& sprite_components);

    components::collision_component build_collision_component();
    
    components::interaction_component build_interaction_component();
    
    components::state_machine_component::state_component build_state();
    components::state_machine_component build_state_machine_component(std::vector<components::state_machine_component::state_component>& state_components);
    components::food_component build_food_component();
}
#endif