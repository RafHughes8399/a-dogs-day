#include "component.h"

Rectangle components::interactor_component::get_interaction_box(Rectangle box) const{
    return Rectangle{box.x - reach_,
                     box.y - reach_,
                     box.width + 2.0f * reach_,
                     box.height + 2.0f * reach_};
}
std::optional<size_t> components::interactor_component::get_target() const{
    return target_;
}
std::vector<size_t> components::interactor_component::get_interactions(){
    return interactions_;
}
void components::interactor_component::interact_with(size_t entity_id){
    target_ = entity_id;
}
void components::interactor_component::stop_interacting(){
    target_ = std::nullopt;
}
