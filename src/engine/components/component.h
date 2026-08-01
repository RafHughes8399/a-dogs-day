#ifndef COMPONENT_H
#define COMPONENT_H

#include <concepts>
#include <queue>
#include <stddef.h>
#include <unordered_map>
#include <utility>
#include <vector>

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
  position_component(Vector2 position, Vector2 direction_scalar)
      : position_(position), direction_scalar_(direction_scalar) {}

private:
  Vector2 position_;
  Vector2 direction_scalar_;
};
class movement_component {
  using path = std::vector<Vector2>;

public:
  ~movement_component() = default;
  movement_component(Vector2 move_speed, std::queue<path> paths = {})
      : paths_(paths), move_speed_(move_speed) {}

private:
  std::queue<path> paths_;
  const Vector2 move_speed_;
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

  private:
    std::vector<sprite::sprite> sprites_;
    size_t sprite_index_;
  };

  ~renderable_component() = default;
  renderable_component(std::vector<sprite_component> sprites = {})
      : sprites_(std::move(sprites)) {}

private:
  std::vector<sprite_component> sprites_;
};
// TODO at a later point, we aren't up to collision and interaction yet even
// currently
class collision_component {
  // defines collision behaviour
};


// for stations to allow for interactions with 
class interaction_component {
  // supports interaction
};

// ! A STATE MACHINE IS A SET OF STATE COMPONENTS
// ! a STATE MACHINE COMPONENT IS A SET OF STATE COMPONENTS
// A STATE MACHINE DEFFINES TRANSITIONS BETWEEN STATES 
    // THE CONDITION TO TRANISITION
    // THE BEHAVIOUR OF TRANSITIONING
    // AND THE NEXT STATE
// bruh we dont 
class state_machine_component{
    class state_component {
        

    };
    //std::vector<state_component>
};
// for the table, the waiter, and the counter and the kitchen station
class food_component {
  // ? current idea for the storage component is for stations to store food,
  // ? tables to store foood
  // ? and dishwasher to store plates
  public:
  private:
};

// menu component ?
// hud component ? 

template <typename C> // C for component
class component_manager {
public:
   void register_compoennt(size_t entity, C component){
        (void) entity;
        (void) component;
   }
   // * only need to unregsiter components when the entity is removed from the game
   void unregister_components(size_t entity_id){
    (void) entity_id;
    
   }
   // get components,
   //
   
private:
    // * one entity to one component. Where an entity needs several of something
    // * (sprites, states) the component itself holds the list - see
    // * renderable_component and state_machine_component.
    std::unordered_map<size_t, C> components_;
};
} // namespace components

// * one manager instance per component type, defined in component_managers.cpp.
// * these live in their own namespace so call sites read
// * managers::renderable_manager_ rather than components::renderable_manager_,
// * which would blur the storage layer into the data layer.
namespace managers {
extern components::component_manager<components::position_component> positional_manager_;
extern components::component_manager<components::movement_component> movment_manager_;
extern components::component_manager<components::renderable_component> renderable_manager_;
extern components::component_manager<components::collision_component> collision_manager_;
extern components::component_manager<components::interaction_component> interaction_manager_;
extern components::component_manager<components::state_machine_component> state_machine_manager_;
extern components::component_manager<components::food_component> food_manager_;
} // namespace managers
#endif