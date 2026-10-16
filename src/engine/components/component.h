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
#include "recipe.hpp"
#include "sprite.h"
#include "state_machine.h"


/**
    header file to define components that make up entities within the game
    currently defines the following components and the usage 
    the collision component - holds the entity hitbox for spatial collision checking
    the storage component - enables an entity to hold items
    the interactor and interactable component - used in conjunction to support interactions between two entities, an interactor can only interact with interactables
    the movement component - supports entity pathfinding
    the position component - gives entities a position in the world
    the renderable component - gives entities a sprite and animation capabilities
    the state machine component - manages the state of an entity for behaivoural and animation purposes
    the key input component - manages 

    components are individually stored by their associated manager which uses a map to 
    tie them to an entity id. hence, the managers act is pseudo parallel arrays where for entity 2
    the postion_mananger[2] and rednerable_manger[2] gives that entity's sprite and position

    @author raffa, september 2026
*/
namespace components {


#define DIRECTIONS 4

class carrier_component {
public:
    ~carrier_component() = default;
    carrier_component(Vector2 previous_position, Vector2* current_position, std::optional<size_t> carried_entity = std::nullopt)
    : previous_position_(previous_position), current_position_(current_position), carried_entity_(carried_entity){}
    carrier_component(const carrier_component& other) = default;
    carrier_component(carrier_component&& other) = default;

    carrier_component& operator=(const carrier_component& other) = default;
    carrier_component& operator=(carrier_component&& other) = default;

    Vector2 get_previous_position() const;
    void set_previous_position(Vector2 position);
    Vector2* get_current_position() const;
    void set_current_position(Vector2* position);
    std::optional<size_t> get_carried_entity() const;
    void set_carried_entity(std::optional<size_t> entity_id);
    bool is_carrying() const;
    void drop();
private:
    Vector2 previous_position_;
    Vector2* current_position_;
    std::optional<size_t> carried_entity_;
};
class clickable_component{
    public:
        ~clickable_component() = default;
        clickable_component(size_t action, int payload = 0)
        : action_(action), payload_(payload){}
        clickable_component(const clickable_component& other) = default;
        clickable_component(clickable_component&& other) = default;

        clickable_component& operator=(const clickable_component& other) = default;
        clickable_component& operator=(clickable_component&& other) = default;

        size_t get_action() const;
        int get_payload() const;
    private:
        size_t action_;
        int payload_;
};
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

class food_component {
public:
    ~food_component() = default;
    food_component(size_t item_id)
    : item_id_(item_id){}
    food_component(const food_component& other) = default;
    food_component(food_component&& other) = default;

    food_component& operator=(const food_component& other) = default;
    food_component& operator=(food_component&& other) = default;

    size_t get_item_id() const;

private:
    size_t item_id_;
};

class hud_component {
public:
    class panel {
    public:
        ~panel() = default;
        panel(sprite::nine_sprite& base, Vector2 offset)
        : base_(base), offset_(offset){}
        panel(const panel& other) = default;
        panel(panel&& other) = default;

        panel& operator=(const panel& other) = delete;
        panel& operator=(panel&& other) = delete;

        sprite::nine_sprite& get_base();
        Vector2 get_offset() const;
    private:
        sprite::nine_sprite base_;
        Vector2 offset_;
    };

    ~hud_component() = default;
    hud_component(sprite::nine_sprite& base, Vector2 offset = Vector2Zero())
    : elements_(), active_(false), panels_(){
        panels_.emplace_back(base, offset);
    }
    hud_component(const hud_component& other) = default;
    hud_component(hud_component&& other) = default;

    hud_component& operator=(const hud_component& other) = delete;
    hud_component& operator=(hud_component&& other) = delete;

    bool is_active() const;
    void set_active(bool active);
    std::vector<panel>& get_panels();
    void add_panel(sprite::nine_sprite& base, Vector2 offset);
    const std::vector<size_t>& get_elements() const;
    void add_element(size_t hud_id);
    void remove_element(size_t hud_id);

private:
    std::vector<size_t> elements_;
    bool active_;
    std::vector<panel> panels_;
};

class interactable_component {
public:

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


    bool can_accept_interactor() const;
    bool has_interactor() const;
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

class owner_component {
public:
    ~owner_component() = default;
    owner_component(size_t owner)
    : owner_(owner){}
    owner_component(const owner_component& other) = default;
    owner_component(owner_component&& other) = default;

    owner_component& operator=(const owner_component& other) = default;
    owner_component& operator=(owner_component&& other) = default;

    size_t get_owner() const;
private:
    size_t owner_;
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
    Vector2* get_position_pointer();
    // only movement_system::update_position should call this - it is what keeps
    // the hitbox and the spatial index in step with the position
    void set_position(Vector2 position);
private:
    Vector2 position_;
};

class recipes_component{
    public:
        ~recipes_component() = default;
        recipes_component(std::vector<recipe::recipe>& recipes, size_t selected_recipe = 0)
        :recipes_(recipes), selected_recipe_(selected_recipe){}
        
        recipes_component(const recipes_component& other) = default;
        recipes_component(recipes_component&& other) = default;

        recipes_component& operator=(const recipes_component& other) = default;
        recipes_component& operator=(recipes_component&& other) = default;

        void select_recipe(size_t index);
        std::vector<recipe::recipe>& get_selected_recipes();
        recipe::recipe& get_selected_recipe();
    private:
        std::vector<recipe::recipe> recipes_;
        size_t selected_recipe_;
};

class renderable_component {
public:
  class body {
  public:
    ~body() = default;
    // TODO (25 / 8 / 26) fix magic number what is 0
    body(std::vector<sprite::sprite> &sprites, size_t index = 0)
        : sprites_(sprites), active_index_(index) {}
    body(const body& other) = default;
    body(body&& other) = default;

    body& operator=(const body& other) = default;
    body& operator=(body&& other) = default;

      sprite::sprite& get_active_sprite();
      std::vector<sprite::sprite>& get_sprites();
      size_t get_active_index() const;
      size_t num_sprites() const;
      void set_index(size_t index);

  private:
    // * holds all the parts of the dog
    std::vector<sprite::sprite> sprites_;
    size_t active_index_;
  };

  ~renderable_component() = default;
  renderable_component(std::vector<body> layers = {})
      : body_(std::move(layers)) {}
  renderable_component(const renderable_component& other) = default;
  renderable_component(renderable_component&& other) = default;

  renderable_component& operator=(const renderable_component& other) = default;
  renderable_component& operator=(renderable_component&& other) = default;

  std::vector<body>& get_layers();
  body* get_body(size_t index);
  size_t num_bodys() const;
  void add_body(body layer);
  void remove_body(size_t index);
  void set_body(size_t index, body layer);
private:
  // * holds the different directions
  std::vector<body> body_;

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

class state_machine_component{
public:
    ~state_machine_component() = default;
    state_machine_component() = default;
    state_machine_component(state_machine::state_machine machine)
    : machine_(std::move(machine)){}
    state_machine_component(const state_machine_component& other) = delete;
    state_machine_component(state_machine_component&& other) = default;

    state_machine_component& operator=(const state_machine_component& other) = delete;
    state_machine_component& operator=(state_machine_component&& other) = default;

    state_machine::state_machine& get_machine();
private:
    state_machine::state_machine machine_;
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

} // namespace components

namespace component_builders{
    components::position_component build_entity_positional_component(Vector2 position);
    components::movement_component build_entity_movement_component(Vector2 move_speed,
        Vector2 direction_scalar = level_config::direction_scalars[level_config::directions::right],
        std::queue<path::path> paths = {});
    components::renderable_component::body build_entity_body(std::vector<sprite::sprite>& sprites, size_t index);
    components::renderable_component build_entity_renderable_component(
        std::vector<components::renderable_component::body>& bodys);

    components::collision_component::hitbox_component build_entity_hitbox_component(std::vector<hitbox::hitbox>& hitboxes, size_t index);
    components::collision_component build_entity_collision_component(
        components::collision_component::hitbox_component hitbox);

    components::interactor_component build_entity_interactor_component(float reach, std::optional<size_t> entity_id = std::nullopt, std::vector<size_t> interactions = {});
    components::interactable_component build_entity_interactable_component(float reach,
        const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets, std::vector<size_t> interactions = {});
    components::key_input_component build_entity_key_input_component(std::vector<game_config::input>& controls);
    components::mouse_input_component build_entity_mouse_input_component(std::vector<game_config::input>& inputs);
    components::state_machine_component build_entity_state_machine_component(state_machine::state_machine machine);
    components::selectable_component build_entity_selectable_component(size_t kind);
    components::storage_component build_entity_storage_component();
    components::food_component build_entity_food_component(size_t item_id);
    components::carrier_component build_entity_carrier_component(Vector2 previous_position, Vector2* current_position, std::optional<size_t> carried_entity = std::nullopt);
    components::recipes_component build_entity_recipes_component(std::vector<recipe::recipe>& recipes);
    components::hud_component build_hud_component(sprite::nine_sprite& base, Vector2 offset = Vector2Zero());

    components::position_component build_hud_positional_component(Vector2 position);
    components::renderable_component::body build_hud_body(std::vector<sprite::sprite>& sprites, size_t index);
    components::renderable_component build_hud_renderable_component(
        std::vector<components::renderable_component::body>& bodys);
    components::collision_component::hitbox_component build_hud_hitbox_component(std::vector<hitbox::hitbox>& hitboxes, size_t index);
    components::collision_component build_hud_collision_component(
        components::collision_component::hitbox_component hitbox);
    components::state_machine_component build_hud_state_machine_component(state_machine::state_machine machine);
    components::clickable_component build_hud_clickable_component(size_t action, int payload = 0);
    components::owner_component build_hud_owner_component(size_t owner);
}

#endif
