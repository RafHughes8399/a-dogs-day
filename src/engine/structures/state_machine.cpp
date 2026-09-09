#include "state_machine.h"

int state_machine::edge::get_transition() const{
    return transition_;
}
size_t state_machine::edge::get_destination() const{
    return destination_;
}

state::state& state_machine::node::get_state(){
    return *state_;
}
const std::vector<state_machine::edge>& state_machine::node::get_edges() const{
    return edges_;
}

size_t state_machine::state_machine::current() const{
    return current_;
}
size_t state_machine::state_machine::num_states() const{
    return graph_.size();
}
state::state* state_machine::state_machine::get_current_state(){
    if(current_ >= graph_.size()){ return nullptr; }
    return &graph_[current_].get_state();
}
std::optional<size_t> state_machine::state_machine::destination_of(int transition) const{
    if(current_ >= graph_.size()){ return std::nullopt; }

    for(const auto& edge : graph_[current_].get_edges()){
        if(edge.get_transition() == transition and edge.get_destination() < graph_.size()){
            return edge.get_destination();
        }
    }
    return std::nullopt;
}
bool state_machine::state_machine::can_transition(int transition) const{
    return destination_of(transition).has_value();
}
void state_machine::state_machine::transition(size_t entity, int transition,
    std::optional<size_t> payload){
    auto destination = destination_of(transition);
    if(not destination.has_value()){ return; }

    graph_[current_].get_state().transitioning_to(entity);
    current_ = destination.value();
    graph_[current_].get_state().on_transitioned_to(entity, payload);
}
void state_machine::state_machine::update(size_t entity, float delta){
    if(current_ >= graph_.size()){ return; }
    graph_[current_].get_state().update(entity, delta);
}
