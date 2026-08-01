#ifndef COMPONENT_H
#define COMPONENT_H

#include <concepts>
#include <queue>
#include <stddef.h>
#include <unordered_map>
#include <vector>

#include "events.h"
#include "events_interface.h"
#include "raylib.h"
#include "raymath.h"
#include "sprite.h"
namespace components {

class position_component {
public:
  ~position_component() = default;
  position_component(size_t component_id, Vector2 position, Vector2 direction_scalar)
      : component_id_(component_id), position_(position), direction_scalar_(direction_scalar) {}

private:
  const size_t component_id_;
  Vector2 position_;
  Vector2 direction_scalar_;
};
class movement_component {
  using path = std::vector<Vector2>;

public:
  ~movement_component() = default;
  movement_component(size_t component_id, Vector2 move_speed, std::queue<path> paths = {})
      : component_id_(component_id), paths_(paths), move_speed_(move_speed) {}

private:
  const size_t component_id_;
  std::queue<path> paths_;
  const Vector2 move_speed_;
};

class sprite_component {
  // body_
public:
  ~sprite_component() = default;
  // TODO fix magic number what is 0
  sprite_component(size_t component_id, std::vector<sprite::sprite> &sprites, size_t index = 0)
      : component_id_(component_id), sprites_(sprites), sprite_index_(index) {}

private:
  const size_t component_id_;
  std::vector<sprite::sprite> sprites_;
  size_t sprite_index_;
};

// * structurally identical to the sprite compoennt, so just give the entity
// mulitple,
// * it is a one to many
// class outline_component {
//     // outline

// };

// class cosmetic_component {
//     // cosmetic slots and the sprites attached
// };
// TODO at a later point, we aren't up to collision and interaction yet even
// currently
class collision_component {
  // defines collision behaviour
};
class interaction_component {
  // supports interaction
};

class state_machine_component {
  // build a state machien for an npc dog
  // what is a state machine if
};
// TODO rename. pooor name is it does not functionally correspond to the storage system
class storage_component {
  // ? current idea for the storage component is for stations to store food,
  // ? tables to store foood
  // ? and dishwasher to store plates
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
    // allows for a one to many relationship, so an entity can
    // have multiple sprite components for exmaple
    std::unordered_map<size_t, std::vector<C>> components_;
};
} // namespace components

// * one manager instance per component type, defined in component_managers.cpp.
// * these live in their own namespace so call sites read managers::sprite_manager_
// * rather than components::sprite_manager_, which would blur the storage layer
// * into the data layer.
namespace managers {
extern components::component_manager<components::position_component> positional_manager_;
extern components::component_manager<components::movement_component> movment_manager_;
extern components::component_manager<components::sprite_component> sprite_manager_;
// extern components::component_manager<components::outline_component> outline_manager_;
// extern components::component_manager<components::cosmetic_component> cosmetic_manager_;
extern components::component_manager<components::collision_component> collision_manager_;
extern components::component_manager<components::interaction_component> interaction_manager_;
extern components::component_manager<components::state_machine_component> state_machine_manager_;
extern components::component_manager<components::storage_component> storage_manager_;
} // namespace managers
#endif