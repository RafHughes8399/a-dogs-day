#ifndef COMPONENT_H
#define COMPONENT_H

#include <algorithm>
#include <concepts>
#include <optional>
#include <queue>
#include <stddef.h>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "path.h"
#include "raylib.h"
#include "raymath.h"
#include "sprite.h"
namespace components {

// * components carry no id of their own - the manager's map key IS the owning
// * entity, so anything iterating components already has it.
// * one hitbox_component per entity. its variants run parallel to the base
// * sprite list, so the two indices are one facing - use
// * component_helpers::set_facing_index.
class collision_component {
public:
  class hitbox_component {
  public:
    ~hitbox_component() = default;
    hitbox_component(std::vector<hitbox::hitbox>& hitboxes, size_t index = 0)
        : hitboxes_(hitboxes), hitbox_index_(index) {}
    hitbox_component(const hitbox_component& other) = default;
    hitbox_component(hitbox_component&& other) = default;

    hitbox_component& operator=(const hitbox_component& other) = default;
    hitbox_component& operator=(hitbox_component&& other) = default;

    std::vector<hitbox::hitbox>& get_hitboxes();
    hitbox::hitbox& get_hitbox();
    size_t get_hitbox_index() const;
    size_t num_hitboxes() const;
    void set_index(size_t index);

  private:
    std::vector<hitbox::hitbox> hitboxes_;
    size_t hitbox_index_;
  };

  ~collision_component() = default;
  collision_component(hitbox_component hitbox)
      : hitbox_component_(std::move(hitbox)) {}
  collision_component(const collision_component& other) = default;
  collision_component(collision_component&& other) = default;

  collision_component& operator=(const collision_component& other) = default;
  collision_component& operator=(collision_component&& other) = default;

  hitbox_component& get_hitbox_component();

private:
  hitbox_component hitbox_component_;
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

// for stations to allow for interactions with

class interactable_component {
public:

    interactable_component() = default;
    Rectangle get_interaction_box(Vector2 position) const;   // inflated by buffer_ based on associated collision component
    bool has_free_slot() const;
    bool is_occupied_by(size_t actor_id) const;
    bool occupy(size_t actor_id);                            // false when full or duplicate
    void release(size_t actor_id);
    const std::vector<size_t>& get_occupants() const;
    size_t get_capacity() const;
private:
    float buffer_; // the amount the hitbox is increased
    size_t capacity_;
    std::vector<size_t> occupants_;
};

// components::interactor_component  - actors do interaction
class interactor_component {
public:
    interactor_component() = default;

    Rectangle get_interaction_box(Vector2 position) const;   // the cell the actor stands in
    bool is_interactor() const { return target_.has_value(); }
    std::optional<size_t> get_target() const;
    void bind(size_t target);
    void unbind();
private:
    float reach_;
    std::optional<size_t> target_;
};


// * player control input component - keyboard bindings only. Mouse input is
// * mouse_input_component; see the note there for why they are not one type.
class key_input_component{
    public:
        ~key_input_component() = default;
        key_input_component(std::vector<game_config::input> controls)
        :controls_(controls){}
        key_input_component(const key_input_component& other) = default;
        key_input_component(key_input_component&& other) = default;

        key_input_component& operator=(const key_input_component& other) = default;
        key_input_component& operator=(key_input_component&& other) = default;

        std::vector<game_config::input>& get_inputs();
    private:
        std::vector<game_config::input> controls_;
};
class mouse_input_component{
    public:
        ~mouse_input_component() = default;
        mouse_input_component(std::vector<game_config::input> inputs)
        :inputs_(inputs){}
        mouse_input_component(const mouse_input_component& other) = default;
        mouse_input_component(mouse_input_component&& other) = default;

        mouse_input_component& operator=(const mouse_input_component& other) = default;
        mouse_input_component& operator=(mouse_input_component&& other) = default;

        std::vector<game_config::input>& get_inputs();
    private:
        std::vector<game_config::input> inputs_;
};

class movement_component {
public:
    ~movement_component() = default;
    // direction defaults to facing right - it is recomputed from
    // position -> next waypoint as soon as there is a path to walk, so the
    // initial value only shows before the entity is given one
    movement_component(Vector2 move_speed,
        Vector2 direction_scalar = level_config::direction_scalars[level_config::directions::right],
        std::queue<path::path> paths = {})
        : paths_(paths), move_speed_(move_speed), direction_scalar_(direction_scalar) {}
    movement_component(const movement_component& other) = default;
    movement_component(movement_component&& other) = default;

    movement_component& operator=(const movement_component& other) = default;
    movement_component& operator=(movement_component&& other) = default;

    bool has_reached_position(Vector2 position);
    path::path& get_current_path();
    std::queue<path::path>& get_paths();
    void append_path(path::path path);
    void set_path(path::path path);
    void clear_paths();
    void finish_path();
    Vector2 get_move_speed();
    Vector2 get_direction_scalar();
    void set_direction_scalar(Vector2 direction_scalar);
private:
    std::queue<path::path> paths_;
    Vector2 move_speed_;
    Vector2 direction_scalar_;
};

// * where a thing is, and nothing else. every system asks this - spatial,
// * collision, rendering - including entities that never move, so it stays the
// * cheapest possible component.
class position_component {
public:
    ~position_component() = default;
    position_component(Vector2 position)
        : position_(position) {}
    position_component(const position_component& other) = default;
    position_component(position_component&& other) = default;

    position_component& operator=(const position_component& other) = default;
    position_component& operator=(position_component&& other) = default;

    Vector2 get_position();
    // only movement_system::update_position should call this - it is what keeps
    // the hitbox and the spatial index in step with the position
    void set_position(Vector2 position);
private:
    Vector2 position_;
};

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
      size_t num_sprites() const;
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

class selectable_component {
public:
    ~selectable_component() = default;
    selectable_component(size_t kind)
    : kind_(kind), is_selected_(false){}
    selectable_component(const selectable_component& other) = default;
    selectable_component(selectable_component&& other) = default;

    selectable_component& operator=(const selectable_component& other) = default;
    selectable_component& operator=(selectable_component&& other) = default;

    bool is_selected() const;
    size_t get_kind() const;
    void select();
    void unselect();
private:
    size_t kind_;
    bool is_selected_;
};

// ! A STATE MACHINE IS A SET OF STATE COMPONENTS
// ! a STATE MACHINE COMPONENT IS A SET OF STATE COMPONENTS
// A STATE MACHINE DEFFINES TRANSITIONS BETWEEN STATES
    // THE CONDITION TO TRANISITION
    // THE BEHAVIOUR OF TRANSITIONING
    // AND THE NEXT STATE
class state_machine_component{
public:
    // virtual dtor - concrete states are held through a base pointer
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

// menu component ?
// hud component ?

} // namespace components

namespace component_managers {
    template <typename C> // C for component
class component_manager {
public:
   ~component_manager() = default;
   component_manager() = default;
   component_manager(const component_manager& other) = default;
   component_manager(component_manager&& other) = default;

   component_manager& operator=(const component_manager& other) = default;
   component_manager& operator=(component_manager&& other) = default;

   // insert_or_assign - operator[] needs a default ctor some components lack
   void register_component(size_t entity, C component){
        components_.insert_or_assign(entity, std::move(component));
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
   auto begin(){return components_.begin();}
   auto end(){return components_.end();}
   auto cbegin() const {return components_.cbegin();}
   auto cend() const {return components_.cend();}
   size_t size() const{
        return components_.size();
   }
   // the managers are globals, so the test harness needs a way back to empty
   // between scenarios
   void clear(){
        components_.clear();
   }
   
private:
    // * one entity to one component. Where an entity needs several of something
    // * (sprites, states) the component itself holds the list - see
    // * renderable_component and state_machine_component.
    std::unordered_map<size_t, C> components_;
};

// * one manager instance per component type, defined in component_managers.cpp.
// * these live in their own namespace so call sites read
// * component_managers::renderable_manager_ rather than components::renderable_manager_,
// * which would blur the storage layer into the data layer.
extern component_manager<components::collision_component> collision_manager_;
extern component_manager<components::key_input_component> control_manager_;
extern component_manager<components::food_component> food_manager_;
extern component_manager<components::interactable_component> interactable_manager_;
extern component_manager<components::interactor_component> interactor_manager_;
extern component_manager<components::mouse_input_component> mouse_input_manager_;
extern component_manager<components::movement_component> movement_manager_;
extern component_manager<components::position_component> positional_manager_;
extern component_manager<components::renderable_component> renderable_manager_;
extern component_manager<components::selectable_component> selectable_manager_;
extern component_manager<components::state_machine_component> state_machine_manager_;
} // namespace component_managers

namespace component_builders{
    components::position_component build_positional_component(Vector2 position);
    components::movement_component build_movement_component(Vector2 move_speed,
        Vector2 direction_scalar = level_config::direction_scalars[level_config::directions::right],
        std::queue<path::path> paths = {});
    components::renderable_component::sprite_component build_sprite_component(std::vector<sprite::sprite>& sprites, size_t index);
    components::renderable_component build_renderable_component(
        std::vector<components::renderable_component::sprite_component>& sprite_components);

    components::collision_component::hitbox_component build_hitbox_component(std::vector<hitbox::hitbox>& hitboxes, size_t index);
    components::collision_component build_collision_component(
        components::collision_component::hitbox_component hitbox);

    components::interactor_component build_interactor_component();
    components::interactable_component build_interactable_component();
    components::key_input_component build_key_input_component(std::vector<game_config::input>& controls);
    components::mouse_input_component build_mouse_input_component(std::vector<game_config::input>& inputs);
    components::state_machine_component::state_component build_state();
    components::state_machine_component build_state_machine_component(std::vector<components::state_machine_component::state_component>& state_components);
    components::food_component build_food_component();
    components::selectable_component build_selectable_component(size_t kind);
}
// thin forwarders to the right manager. inline because this header lands in
// several TUs
namespace component_helpers{
    inline void register_positional_component(size_t entity_id, components::position_component component){
        component_managers::positional_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_movement_component(size_t entity_id, components::movement_component component){
        component_managers::movement_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_renderable_component(size_t entity_id, components::renderable_component component){
        component_managers::renderable_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_collision_component(size_t entity_id, components::collision_component component){
        component_managers::collision_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_interactor_component(size_t entity_id, components::interactor_component component){
        component_managers::interactor_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_interactable_compoennt(size_t entity_id, components::interactable_component component){
        component_managers::interactable_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_key_input_component(size_t entity_id, components::key_input_component component){
        component_managers::control_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_mouse_input_component(size_t entity_id, components::mouse_input_component component){
        component_managers::mouse_input_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_state_machine_component(size_t entity_id, components::state_machine_component component){
        component_managers::state_machine_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_food_component(size_t entity_id, components::food_component component){
        component_managers::food_manager_.register_component(entity_id, std::move(component));
    }
    inline void register_selectable_component(size_t entity_id, components::selectable_component component){
        component_managers::selectable_manager_.register_component(entity_id, std::move(component));
    }
    inline bool is_mouse_positioned(size_t entity_id){
        return component_managers::mouse_input_manager_.get_component(entity_id) != nullptr;
    }
    // writes sprite and hitbox indices together. missing either component is fine

    inline void set_facing_index(size_t entity_id, size_t index){
        if(auto* renderable = component_managers::renderable_manager_.get_component(entity_id)){
            for(auto& sprite_component : renderable->get_sprites()){
                if(index < sprite_component.num_sprites()){
                    sprite_component.set_index(index);
                }
            }
        }
        if(auto* collision = component_managers::collision_manager_.get_component(entity_id)){
            auto& hitboxes = collision->get_hitbox_component();
            if(index < hitboxes.num_hitboxes()){
                hitboxes.set_index(index);
            }
        }
    }

    inline void unregister_positional_component(size_t entity_id){
        component_managers::positional_manager_.unregister_component(entity_id);
    }
    inline void unregister_movement_component(size_t entity_id){
        component_managers::movement_manager_.unregister_component(entity_id);
    }
    inline void unregister_renderable_component(size_t entity_id){
        component_managers::renderable_manager_.unregister_component(entity_id);
    }
    inline void unregister_collision_component(size_t entity_id){
        component_managers::collision_manager_.unregister_component(entity_id);
    }
    inline void unregister_interactor_component(size_t entity_id){
        component_managers::interactor_manager_.unregister_component(entity_id);
    }
    inline void unregister_interactable_component(size_t entity_id){
        component_managers::interactable_manager_.unregister_component(entity_id);
    }
    inline void unregister_key_input_component(size_t entity_id){
        component_managers::control_manager_.unregister_component(entity_id);
    }
    inline void unregister_mouse_input_component(size_t entity_id){
        component_managers::mouse_input_manager_.unregister_component(entity_id);
    }
    inline void unregister_state_machine_component(size_t entity_id){
        component_managers::state_machine_manager_.unregister_component(entity_id);
    }
    inline void unregister_food_component(size_t entity_id){
        component_managers::food_manager_.unregister_component(entity_id);
    }
    inline void unregister_selectable_component(size_t entity_id){
        component_managers::selectable_manager_.unregister_component(entity_id);
    }

    // blanket teardown - erase on a missing key is a no-op, so this is correct
    // for every entity kind without tracking what a builder registered
    inline void unregister_all_components(size_t entity_id){
        unregister_positional_component(entity_id);
        unregister_movement_component(entity_id);
        unregister_renderable_component(entity_id);
        unregister_collision_component(entity_id);
        unregister_interactor_component(entity_id);
        unregister_interactable_component(entity_id);
        unregister_key_input_component(entity_id);
        unregister_mouse_input_component(entity_id);
        unregister_state_machine_component(entity_id);
        unregister_food_component(entity_id);
        unregister_selectable_component(entity_id);
    }

    // total components registered across every manager
    inline size_t num_registered_components(size_t entity_id){
        size_t count = 0;
        count += component_managers::positional_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::movement_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::renderable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::collision_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::interactor_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::interactable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::control_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::mouse_input_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::state_machine_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::food_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        count += component_managers::selectable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
        return count;
    }

    // wipes every manager - for the test harness between scenarios
    inline void clear_all_components(){
        component_managers::positional_manager_.clear();
        component_managers::movement_manager_.clear();
        component_managers::renderable_manager_.clear();
        component_managers::collision_manager_.clear();
        component_managers::interactor_manager_.clear();
        component_managers::interactable_manager_.clear();
        component_managers::control_manager_.clear();
        component_managers::mouse_input_manager_.clear();
        component_managers::state_machine_manager_.clear();
        component_managers::food_manager_.clear();
        component_managers::selectable_manager_.clear();
    }
}
#endif