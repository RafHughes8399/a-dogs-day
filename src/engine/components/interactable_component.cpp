#include "component.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include "system.h"
#include <string>

Rectangle components::interactable_component::get_interaction_box(Rectangle box) const{
    return Rectangle{box.x - reach_,
                     box.y - reach_,
                     box.width + 2.0f * reach_,
                     box.height + 2.0f * reach_};
}
std::optional<Vector2> components::interactable_component::get_interaction_offset(Vector2 source, Vector2 own_position) const{
    static const char* direction_names[DIRECTIONS] = {"left", "right", "up", "down"};
    std::optional<size_t> closest_index = std::nullopt;
    float closest_distance = 0.0f;
    for(size_t i = 0; i < positions_.size(); ++i){
        if(not positions_[i].has_value()){ continue; }
        auto offset = positions_[i].value();
        auto position = Vector2Add(own_position, offset);
        auto* node = systems::movement_system::get_instance().node_at(position);
        if(node == nullptr){
            debug::log("[interactable_component::get_interaction_offset] direction: "
                + std::string(direction_names[i]) + ", position: " + raglib::vector_to_string(position)
                + ", REJECTED - off the walkable grid");
            continue;
        }
        if(not node->entities_.empty()){
            std::string occupants;
            for(auto occupant : node->entities_){
                occupants += (occupants.empty() ? "" : ", ") + std::to_string(occupant);
            }
            debug::log("[interactable_component::get_interaction_offset] direction: "
                + std::string(direction_names[i]) + ", position: " + raglib::vector_to_string(position)
                + ", node_id: " + std::to_string(node->id_) + ", REJECTED - occupied by entity "
                + occupants);
            continue;
        }
        auto distance = Vector2Distance(source, position);
        debug::log("[interactable_component::get_interaction_offset] direction: "
            + std::string(direction_names[i]) + ", position: " + raglib::vector_to_string(position)
            + ", node_id: " + std::to_string(node->id_) + ", distance_from_source: " + std::to_string(distance));
        if(not closest_index.has_value() or distance < closest_distance){
            closest_index = i;
            closest_distance = distance;
        }
    }
    std::optional<Vector2> closest_offset = closest_index.has_value()
        ? positions_[closest_index.value()]
        : std::nullopt;
    debug::log(closest_offset.has_value()
        ? "[interactable_component::get_interaction_offset] chosen offset: " + raglib::vector_to_string(closest_offset.value())
        : "[interactable_component::get_interaction_offset] no free interaction slot found");
    return closest_offset;
}
bool components::interactable_component::can_accept_interactor() const{
    for(size_t i = 0; i < positions_.size(); ++i){
        if(positions_[i].has_value() and not interactors_[i].has_value()){
            return true;
        }
    }
    return false;
}
bool components::interactable_component::claim(size_t interactor_id){
    for(auto& interactor : interactors_){
        if(interactor.has_value() and interactor.value() == interactor_id){
            return false;
        }
    }
    for(size_t i = 0; i < positions_.size(); ++i){
        if(positions_[i].has_value() and not interactors_[i].has_value()){
            interactors_[i] = interactor_id;
            return true;
        }
    }
    return false;
}
void components::interactable_component::release(size_t interactor_id){
    for(auto& interactor : interactors_){
        if(interactor.has_value() and interactor.value() == interactor_id){
            interactor.reset();
        }
    }
}
const std::array<std::optional<size_t>, DIRECTIONS>& components::interactable_component::get_interactors() const{
    return interactors_;
}
