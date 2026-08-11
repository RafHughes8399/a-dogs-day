#include "component.h"

// Definitions for the extern manager declarations in component.h - one instance
// per component type. Plain globals, in the same spirit as entities::e_builder
// and events::global_dispatcher_, not singletons.
//
// These MUST sit in the same namespace as the externs in component.h. Defined
// under a different namespace (or at global scope) they become unrelated
// variables that merely share a name, and the externs stay undefined until
// something references one and the link fails.

// variables, not functions - the qualified ns::name form used in the builders
// is not available, so a namespace block is the only way to scope these
namespace component_managers {
component_manager<components::position_component> positional_manager_;
component_manager<components::movement_component> movement_manager_;
component_manager<components::renderable_component> renderable_manager_;
component_manager<components::collision_component> collision_manager_;
component_manager<components::interaction_component> interaction_manager_;
component_manager<components::key_input_component> control_manager_;
component_manager<components::mouse_input_component> mouse_input_manager_;
component_manager<components::state_machine_component> state_machine_manager_;
component_manager<components::food_component> food_manager_;
component_manager<components::selectable_component> selectable_manager_;
}

