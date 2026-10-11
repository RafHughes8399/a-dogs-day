#ifndef COMPONENT_MANAGERS_HPP
#define COMPONENT_MANAGERS_HPP

#include "component.h"

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
        components_.erase(entity);
        components_.emplace(entity, std::move(component));
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
   bool contains(size_t entity) const{
        return components_.find(entity) != components_.end();
   }
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
// * component_managers::entity_renderable_manager_ rather than components::entity_renderable_manager_,
// * which would blur the storage layer into the data layer.
extern component_manager<components::carrier_component> entity_carrier_manager_;
extern component_manager<components::collision_component> entity_collision_manager_;
extern component_manager<components::food_component> entity_food_manager_;
extern component_manager<components::key_input_component> entity_control_manager_;
extern component_manager<components::interactable_component> entity_interactable_manager_;
extern component_manager<components::interactor_component> entity_interactor_manager_;
extern component_manager<components::mouse_input_component> entity_mouse_input_manager_;
extern component_manager<components::movement_component> entity_movement_manager_;
extern component_manager<components::position_component> entity_positional_manager_;
extern component_manager<components::recipes_component> entity_recipes_manager_;
extern component_manager<components::renderable_component> entity_renderable_manager_;
extern component_manager<components::selectable_component> entity_selectable_manager_;
extern component_manager<components::state_machine_component> entity_state_machine_manager_;
extern component_manager<components::storage_component> entity_storage_manager_;

extern component_manager<components::hud_component> hud_manager_;
extern component_manager<components::position_component> hud_positional_manager_;
extern component_manager<components::renderable_component> hud_renderable_manager_;
extern component_manager<components::collision_component> hud_collision_manager_;
extern component_manager<components::state_machine_component> hud_state_machine_manager_;
extern component_manager<components::clickable_component> hud_clickable_manager_;
extern component_manager<components::owner_component> hud_owner_manager_;
} // namespace component_managers

#endif
