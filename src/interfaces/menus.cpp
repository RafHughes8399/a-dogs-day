#include "menus.h"

size_t menus::menu_graph::node::get_id() const{
    return id_;
}
const std::vector<size_t>& menus::menu_graph::node::get_roots(size_t group) const{
    static const std::vector<size_t> none{};
    return group < roots_.size() ? roots_[group] : none;
}
std::vector<size_t> menus::menu_graph::node::get_all_roots() const{
    std::vector<size_t> all;
    for(auto& group : roots_){
        all.insert(all.end(), group.begin(), group.end());
    }
    return all;
}
std::optional<size_t> menus::menu_graph::node::get_active_root() const{
    if(roots_[group_].empty()){ return std::nullopt; }
    return roots_[group_][active_[group_]];
}
size_t menus::menu_graph::node::get_group() const{
    return group_;
}
void menus::menu_graph::node::set_group(size_t group){
    ensure_group(group);
    group_ = group;
}
void menus::menu_graph::node::add_root(size_t root, size_t group){
    ensure_group(group);
    roots_[group].push_back(root);
}
void menus::menu_graph::node::next_root(){
    if(active_[group_] + 1 < roots_[group_].size()){ ++active_[group_]; }
}
void menus::menu_graph::node::previous_root(){
    if(active_[group_] > 0){ --active_[group_]; }
}
void menus::menu_graph::node::clear(){
    roots_.assign(1, std::vector<size_t>{});
    active_.assign(1, 0);
    group_ = 0;
}
void menus::menu_graph::node::ensure_group(size_t group){
    if(group < roots_.size()){ return; }
    roots_.resize(group + 1);
    active_.resize(group + 1, 0);
}

int menus::menu_graph::edge::get_key() const{
    return key_;
}
size_t menus::menu_graph::edge::get_destination() const{
    return destination_;
}

void menus::menu_graph::build_graph(){
    for(size_t id = 0; id < static_cast<size_t>(menu_ids_size); ++id){
        graph_.push_back(std::make_pair(node(id), std::vector<edge>{}));
    }
    graph_[menu_ids::blank].second = {
        edge(game_config::key_press_actions::back, menu_ids::pause),
        edge(game_config::key_press_actions::pause, menu_ids::pause),
        edge(game_config::key_press_actions::menu_open, menu_ids::tab),
        edge(game_config::key_press_actions::shop_open, menu_ids::shop),
        edge(game_config::key_press_actions::quests_open, menu_ids::quest),
        edge(game_config::key_press_actions::inventory_open, menu_ids::inventory),
        edge(game_config::key_press_actions::map_open, menu_ids::map),
    };
    graph_[menu_ids::pause].second = { edge(game_config::key_press_actions::back, menu_ids::blank), 
                                        edge(game_config::key_press_actions::pause, menu_ids::blank) };
    graph_[menu_ids::tab].second = { edge(game_config::key_press_actions::back, menu_ids::blank),
                                        edge(game_config::key_press_actions::menu_open, menu_ids::blank) };
    graph_[menu_ids::shop].second = { edge(game_config::key_press_actions::back, menu_ids::blank),
                                        edge(game_config::key_press_actions::shop_open, menu_ids::blank) };
    graph_[menu_ids::quest].second = { edge(game_config::key_press_actions::back, menu_ids::blank),
                                        edge(game_config::key_press_actions::quests_open, menu_ids::blank) };
    graph_[menu_ids::inventory].second = { edge(game_config::key_press_actions::back, menu_ids::blank),
                                        edge(game_config::key_press_actions::inventory_open, menu_ids::blank) };
    graph_[menu_ids::map].second = { edge(game_config::key_press_actions::back, menu_ids::blank),
                                    edge(game_config::key_press_actions::map_open, menu_ids::blank) };
}

size_t menus::menu_graph::navigate(int key){
    for(auto& route : graph_[current_].second){
        if(route.get_key() != key){ continue; }
        current_ = route.get_destination();
        break;
    }
    return current_;
}
void menus::menu_graph::add_root(size_t menu, size_t root, size_t group){
    graph_.at(menu).first.add_root(root, group);
}
void menus::menu_graph::reset(){
    for(auto& entry : graph_){
        entry.first.clear();
    }
    current_ = menu_ids::blank;
}
menus::menu_graph::node& menus::menu_graph::get_node(size_t menu){
    return graph_.at(menu).first;
}
