#include "component.h"

// Definitions for the extern manager declarations in component.h - one instance
// per component type. Plain globals, in the same spirit as entities::e_builder
// and events::global_dispatcher_, not singletons.
//
// These MUST sit in the same namespace as the externs in component.h. Defined
// under a different namespace (or at global scope) they become unrelated
// variables that merely share a name, and the externs stay undefined until
// something references one and the link fails.
namespace managers {

components::component_manager<components::position_component> positional_manager_;
components::component_manager<components::movement_component> movment_manager_;
components::component_manager<components::renderable_component> renderable_manager_;
components::component_manager<components::collision_component> collision_manager_;
components::component_manager<components::interaction_component> interaction_manager_;
components::component_manager<components::state_machine_component> state_machine_manager_;
components::component_manager<components::food_component> food_manager_;

} // namespace managers
