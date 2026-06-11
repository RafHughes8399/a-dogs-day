#include "menus.h"
#include <iostream>

// define the builder 
menus::menu_builder menus::m_builder_;


// ------------------------------- menu --------------------------------------- //
void menus::menu::render(){
    // TODO
    DrawRectangle(static_cast<int>(box_.x), static_cast<int>(box_.y),
        static_cast<int>(box_.width), static_cast<int>(box_.height), YELLOW);
    //DrawText(text_.c_str(), box_.x + 32, box_.y + 32, 32, BLACK);
    return;
}

void menus::item_menu::render(){
    return ;
}

void menus::menu::subscribe_hud(){
    components_.buttons_subscribe();
}
void menus::menu::unsubscribe_hud(){
    components_.buttons_unsubscribe();

}
// ------------------------------ builder ---------------------------------- // 
std::unique_ptr<menus::menu> menus::menu_builder::build_blank_menu() {
    std::cout << "[menu_builder] construct blank menu" << std::endl;
    return std::make_unique<menus::menu>(Rectangle{-1, -1, 0, 0}, hud::hud());
}
std::unique_ptr<menus::menu> menus::menu_builder::build_pause_menu(){
    std::cout << "[menu_builder] construct pause menu" << std::endl;
    std::string text = "pause";
    Rectangle box = { 400, 400,  200, 200};
    return std::make_unique<menus::menu>(box, hud::hud());
}
std::unique_ptr<menus::menu> menus::menu_builder::build_tab_menu(){
    std::cout << "[menu_builder] construct tab menu" << std::endl;
    std::string text = "tab";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, hud::hud());
    
}
std::unique_ptr<menus::menu> menus::menu_builder::build_shop_menu(){
    std::cout << "[menu_builder] construct shop menu" << std::endl;
    std::string text = "shop";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, hud::hud());
}
std::unique_ptr<menus::menu> menus::menu_builder::build_map_menu(){
    std::cout << "[menu_builder] construct map menu" << std::endl;
    std::string text = "map";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, hud::hud());
}
std::unique_ptr<menus::menu> menus::menu_builder::build_quest_menu(){
    std::cout << "[menu_builder] construct quest menu" << std::endl;
    std::string text = "quest";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, hud::hud());
}
std::unique_ptr<menus::menu> menus::menu_builder::build_inventory_menu(){
    std::cout << "[menu_builder] construct inventory menu" << std::endl;
    std::string text = "inventory";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, hud::hud());    
}
menus::menu_graph menus::menu_builder::build_menus(){
    std::cout << "[menu_builder build]: return menu graph" << std::endl;
    return menu_graph();
}
// ------------------------------ graph --------------------------------------- // 
menus::menu_graph::edge menus::menu_graph::build_edge(node* dst, int key){
    return edge {key, dst};
}
menus::menu_graph::node menus::menu_graph::build_node(std::unique_ptr<menu> menu, size_t id){
    return node {std::move(menu), id};
}
void menus::menu_graph::build_graph(){
    std::cout << "[menu_graph build] construct nodes " << std::endl;
    graph_.push_back(std::make_pair(build_node(m_builder_.build_blank_menu(), menu_ids::blank), std::vector<edge>{}));
    std::cout << "[menu_graph build] constructed blank menu " << std::endl;
    graph_.push_back(std::make_pair(build_node(m_builder_.build_pause_menu(), menu_ids::pause), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_tab_menu(), menu_ids::tab), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_inventory_menu(), menu_ids::inventory), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_map_menu(), menu_ids::map), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_shop_menu(), menu_ids::shop), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_quest_menu(), menu_ids::quest), std::vector<edge>{}));
    
    // Now add edges using pointers to nodes in the graph
    // Blank menu edges
    std::cout << "[menu_graph build] construct edges " << std::endl;
    graph_[menu_ids::blank].second = {
        build_edge(&graph_[menu_ids::pause].first, controls_config::key_press_actions::back),
        build_edge(&graph_[menu_ids::tab].first, controls_config::key_press_actions::menu_open),
        build_edge(&graph_[menu_ids::shop].first, controls_config::key_press_actions::shop_open),
        build_edge(&graph_[menu_ids::quest].first, controls_config::key_press_actions::quests_open),
        build_edge(&graph_[menu_ids::inventory].first, controls_config::key_press_actions::inventory_open),
        build_edge(&graph_[menu_ids::map].first, controls_config::key_press_actions::map_open),
    };
    
    std::cout << "[menu_graph build] construct edges " << std::endl;
    // Other menus all go back to blank
    graph_[menu_ids::pause].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::tab].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::shop].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::quest].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::inventory].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::map].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
}



// ------------------------------- menu graph -------------------------------------- //
int menus::menu_graph::update(float delta){
    (void) delta;
    return 0;
}

void menus::menu_graph::on_menu_interact_event(const events::interact_menu& event){
    auto cursor_hitbox = event.get_hitbox();
    // check the buttons in the current menu
    // ? and maybe interactable hud componenents
    
    // check_menu_interaction
    //auto buttons = graph_[current_].first.menu_->get_buttons();
    // check buttons 
    //cursor_hitbox.check_collision(button.get_hitbox());

    return;
}
void menus::menu_graph::on_key_press_event(const events::key_press& event){
    int key = event.get_key();
    // check the edges of current, if there is a key match, "move" 
    size_t old_current = current_;
    for(auto & edge : graph_[current_].second){
        if(edge.key_ == key){
            node* dst = edge.destination_menu_;
            current_ = dst->id_;
        }
    }
    size_t new_current = current_;

    // TODO unsub old current buttons, sub new current buttons
    graph_[old_current].first.menu_->unsubscribe_hud();
    graph_[new_current].first.menu_->subscribe_hud();
    // at this point current has changed

}

void menus::menu_graph::render(){
    graph_[current_].first.menu_->render();
}
