#include "component.h"

namespace component_managers {
component_manager<components::collision_component> collision_manager_;
component_manager<components::key_input_component> control_manager_;
component_manager<components::interactable_component> interactable_manager_;
component_manager<components::interactor_component> interactor_manager_;
component_manager<components::mouse_input_component> mouse_input_manager_;
component_manager<components::movement_component> movement_manager_;
component_manager<components::position_component> positional_manager_;
component_manager<components::renderable_component> renderable_manager_;
component_manager<components::selectable_component> selectable_manager_;
component_manager<components::state_machine_component> state_machine_manager_;
component_manager<components::storage_component> storage_manager_;
} // namespace component_managers
