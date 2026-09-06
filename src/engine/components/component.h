#ifndef COMPONENT_H
#define COMPONENT_H

#include <array>
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
#include "item_stack.hpp"
#include "path.h"
#include "raylib.h"
#include "raymath.h"
#include "sprite.h"
namespace components {


#define DIRECTIONS 4
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
class storage_component {
  // ? current idea for the storage component is for stations to store food,
  // ? tables to store foood
  // ? and dishwasher to store plates
  public:
    ~storage_component() = default;
    storage_component(item_stack::item_stack items)
    :items_(items){}
    storage_component(const storage_component& other) = default;
    storage_component(storage_component&& other) = default;

    storage_component& operator=(const storage_component& other) = default;
    storage_component& operator=(storage_component&& other) = default;

    bool empty() const;
    size_t size() const;
    item_stack::item& head();
    size_t take();
    void place(size_t item_id);

  private:
    item_stack::item_stack items_;
};
class interactable_component {
public:
    // to solve the problem where an entity is sent to a station but cannot pathfind
    // becuase the raw click input is a blocked node on the graph. instead, we resolve
    // an interaction offset against the station's own live position at query time -
    // the offset is from the station origin, not a world position: a station can be
    // moved, and a derived position has no sync path to get wrong.

    // * the interaction positions require the existance of a collision component 
    // * hence, the offsets arrays are simply Vector2Zeros() when passed in as the arrya
    // * this consttructor's precondtion is an existing collision component and thus
    // * performs the offset calcualtions in its body
    ~interactable_component() = default;
    interactable_component(float reach, std::array<std::optional<Vector2>, DIRECTIONS> positions, std::vector<size_t> interactions = {})
    : reach_(reach), positions_(positions), interactors_(), interactions_(interactions){
        
    }
    interactable_component(const interactable_component& other) = default;
    interactable_component(interactable_component&& other) = default;
    
    interactable_component& operator=(const interactable_component& other) = default;
    interactable_component& operator=(interactable_component&& other) = default;
    
    std::vector<size_t> get_interactions();
    Rectangle get_interaction_box(Rectangle box) const;
    std::optional<Vector2> get_interaction_offset(Vector2 source, Vector2 own_position) const;
    // * occupancy is a claim, not a proximity test - a table promised to a
    // * customer still walking over reads as taken, which a spatial check
    // * cannot express. claim/release are the only writers of interactors_,
    // * and their counterparts on interactor_component are interact_with/
    // * stop_interacting - the two sides are paired only in
    // * component_helpers::unregister_interact*_component
    bool can_accept_interactor() const;
    bool claim(size_t interactor_id);
    void release(size_t interactor_id);
    const std::array<std::optional<size_t>, DIRECTIONS>& get_interactors() const;
#ifdef DOG_DAYS_TESTING
    std::optional<Vector2> get_slot_offset(size_t direction) const{
        return positions_[direction];
    }
#endif
private:
    float reach_;
    // order is as defined in the config enum
    // * 0. left
    // * 1. right
    // * 2. up
    // * 3. down
    std::array<std::optional<Vector2>, DIRECTIONS> positions_;
    // parallel to positions_ - interactors_[i] holds whoever claimed slot i
    std::array<std::optional<size_t>, DIRECTIONS> interactors_;
    std::vector<size_t> interactions_;
};

// components::interactor_component  - actors do interaction
class interactor_component {
    public:
    ~interactor_component() = default;
    interactor_component(float reach, std::optional<size_t> entity_id = std::nullopt, std::vector<size_t> interactions = {})
    : reach_(reach), target_(entity_id), interactions_(interactions){}
    interactor_component(const interactor_component& other) = default;
    interactor_component(interactor_component&& other) = default;
    
    interactor_component& operator=(const interactor_component& other) = default;
    interactor_component& operator=(interactor_component&& other) = default;
    
    Rectangle get_interaction_box(Rectangle box) const;
    bool is_interacting() const { return target_.has_value(); }
    std::optional<size_t> get_target() const;
    void interact_with(size_t entity_id);
    void stop_interacting();
    std::vector<size_t> get_interactions();
    
    private:
        float reach_;
        std::optional<size_t> target_;
        std::vector<size_t> interactions_;
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
  class sprite_layer {
  public:
    ~sprite_layer() = default;
    // TODO (25 / 8 / 26) fix magic number what is 0
    sprite_layer(std::vector<sprite::sprite> &sprites, size_t index = 0)
        : sprites_(sprites), active_index_(index) {}
    sprite_layer(const sprite_layer& other) = default;
    sprite_layer(sprite_layer&& other) = default;

    sprite_layer& operator=(const sprite_layer& other) = default;
    sprite_layer& operator=(sprite_layer&& other) = default;

      sprite::sprite& get_active_sprite();
      std::vector<sprite::sprite>& get_sprites();
      size_t get_active_index() const;
      size_t num_sprites() const;
      void set_index(size_t index);

  private:
    std::vector<sprite::sprite> sprites_;
    size_t active_index_;
  };

  ~renderable_component() = default;
  renderable_component(std::vector<sprite_layer> layers = {})
      : layers_(std::move(layers)) {}
  renderable_component(const renderable_component& other) = default;
  renderable_component(renderable_component&& other) = default;

  renderable_component& operator=(const renderable_component& other) = default;
  renderable_component& operator=(renderable_component&& other) = default;

  std::vector<sprite_layer>& get_layers();
  sprite_layer* get_sprite_layer(size_t index);
  size_t num_sprite_layers() const;
  void add_sprite_layer(sprite_layer layer);
  void remove_sprite_layer(size_t index);
  void set_sprite_layer(size_t index, sprite_layer layer);
private:
  std::vector<sprite_layer> layers_;

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
    // TODO 30 / 8 / 26 
    // * states affect behaviour
    // * state machines define the transitional logic [ akin to how the menu graph is setup]
    // * best way to structure this logic:
    // * i think sub states is good
    // the entity needs state, and then the state machine assigns a behaviour to a state and performs it
    // maps the state id to the state behaviour or characteristic ? 
class state_machine_component{
public:
    class state_component {
    public:
        ~state_component() = default;
        state_component() = default;
        state_component(const state_component& other) = default;
        state_component(state_component&& other) = default;

        state_component& operator=(const state_component& other) = default;
        state_component& operator=(state_component&& other) = default;

        
    private:
        size_t state_id_;
        state_component * next_state_;
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
extern component_manager<components::interactable_component> interactable_manager_;
extern component_manager<components::interactor_component> interactor_manager_;
extern component_manager<components::mouse_input_component> mouse_input_manager_;
extern component_manager<components::movement_component> movement_manager_;
extern component_manager<components::position_component> positional_manager_;
extern component_manager<components::renderable_component> renderable_manager_;
extern component_manager<components::selectable_component> selectable_manager_;
extern component_manager<components::state_machine_component> state_machine_manager_;
extern component_manager<components::storage_component> storage_manager_;
} // namespace component_managers

namespace component_builders{
    components::position_component build_positional_component(Vector2 position);
    components::movement_component build_movement_component(Vector2 move_speed,
        Vector2 direction_scalar = level_config::direction_scalars[level_config::directions::right],
        std::queue<path::path> paths = {});
    components::renderable_component::sprite_layer build_sprite_layer(std::vector<sprite::sprite>& sprites, size_t index);
    components::renderable_component build_renderable_component(
        std::vector<components::renderable_component::sprite_layer>& sprite_layers);

    components::collision_component::hitbox_component build_hitbox_component(std::vector<hitbox::hitbox>& hitboxes, size_t index);
    components::collision_component build_collision_component(
        components::collision_component::hitbox_component hitbox);

    components::interactor_component build_interactor_component(float reach, std::optional<size_t> entity_id = std::nullopt, std::vector<size_t> interactions = {});
    components::interactable_component build_interactable_component(float reach,
        const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets, std::vector<size_t> interactions = {});
    components::key_input_component build_key_input_component(std::vector<game_config::input>& controls);
    components::mouse_input_component build_mouse_input_component(std::vector<game_config::input>& inputs);
    components::state_machine_component::state_component build_state();
    components::state_machine_component build_state_machine_component(std::vector<components::state_machine_component::state_component>& state_components);
    components::selectable_component build_selectable_component(size_t kind);
    components::storage_component build_storage_component();
}
// thin forwarders to the right manager; defined in component_helpers.cpp
namespace component_helpers{
    void register_positional_component(size_t entity_id, components::position_component component);
    void register_movement_component(size_t entity_id, components::movement_component component);
    void register_renderable_component(size_t entity_id, components::renderable_component component);
    void register_collision_component(size_t entity_id, components::collision_component component);
    void register_interactor_component(size_t entity_id, components::interactor_component component);
    void register_interactable_component(size_t entity_id, components::interactable_component component);
    void register_key_input_component(size_t entity_id, components::key_input_component component);
    void register_mouse_input_component(size_t entity_id, components::mouse_input_component component);
    void register_state_machine_component(size_t entity_id, components::state_machine_component component);
    void register_selectable_component(size_t entity_id, components::selectable_component component);
    void register_storage_component(size_t entity_id, components::storage_component component);

    void add_positional_component(size_t entity_id, Vector2 position);
    void add_movement_component(size_t entity_id, Vector2 move_speed,
        Vector2 direction_scalar = level_config::direction_scalars[level_config::directions::right],
        std::queue<path::path> paths = {});
    void add_renderable_component(size_t entity_id,
        std::vector<components::renderable_component::sprite_layer>& sprite_layers);
    void add_collision_component(size_t entity_id,
        components::collision_component::hitbox_component hitbox);
    void add_interactor_component(size_t entity_id, float reach,
        std::optional<size_t> target_entity_id = std::nullopt, std::vector<size_t> interactions = {});
    void add_interactable_component(size_t entity_id, float reach,
        const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets, std::vector<size_t> interactions = {});
    void add_key_input_component(size_t entity_id, std::vector<game_config::input>& controls);
    void add_mouse_input_component(size_t entity_id, std::vector<game_config::input>& inputs);
    void add_state_machine_component(size_t entity_id,
        std::vector<components::state_machine_component::state_component>& state_components);
    void add_selectable_component(size_t entity_id, size_t kind);
    void add_storage_component(size_t entity_id);
    void add_stored_item(size_t entity_id, size_t slot, size_t item_id);
    std::optional<size_t> take_stored_item(size_t entity_id, size_t slot);
    void update_item_sprite(size_t entity_id, size_t slot);

    void create_offset_position_list(Rectangle box, std::array<std::optional<Vector2>, DIRECTIONS>& positions);
    bool is_mouse_positioned(size_t entity_id);
    void set_facing_index(size_t entity_id, size_t index);
    void set_sprite_index(size_t entity_id, size_t slot, size_t index);

    void unregister_positional_component(size_t entity_id);
    void unregister_movement_component(size_t entity_id);
    void unregister_renderable_component(size_t entity_id);
    void unregister_collision_component(size_t entity_id);
    void unregister_interactor_component(size_t entity_id);
    void unregister_interactable_component(size_t entity_id);
    void unregister_key_input_component(size_t entity_id);
    void unregister_mouse_input_component(size_t entity_id);
    void unregister_state_machine_component(size_t entity_id);
    void unregister_selectable_component(size_t entity_id);
    void unregister_storage_component(size_t entity_id);
    void unregister_all_components(size_t entity_id);

    size_t num_registered_components(size_t entity_id);
    void clear_all_components();
}
#endif
