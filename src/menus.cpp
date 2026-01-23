#include "menus.h"

// define the builder 
menus::menu_builder menus::m_builder_;


// ------------------------------- menu --------------------------------------- //
void menus::menu::render(){
    // TODO
    DrawRectangle(box_.x, box_.y, box_.width, box_.height, YELLOW);
    DrawText(text_.c_str(), box_.x + 32, box_.y + 32, 32, BLACK);
    return;
}

void menus::item_menu::render(){
    return ;
}
// ------------------------------ builder ---------------------------------- // 
std::unique_ptr<menus::menu> menus::menu_builder::build_blank_menu() {
    
    return std::make_unique<menus::menu>(Rectangle{-1, -1, 0, 0});
}
std::unique_ptr<menus::menu> menus::menu_builder::build_pause_menu(){
    std::string text = "pause";
    Rectangle box = { 400, 400,  200, 200};
    return std::make_unique<menus::menu>(box, text);
}
std::unique_ptr<menus::menu> menus::menu_builder::build_tab_menu(){
    std::string text = "tab";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, text);
    
}
std::unique_ptr<menus::menu> menus::menu_builder::build_shop_menu(){
    std::string text = "shop";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, text);
}
std::unique_ptr<menus::menu> menus::menu_builder::build_map_menu(){
    std::string text = "map";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, text);
}
std::unique_ptr<menus::menu> menus::menu_builder::build_quest_menu(){
    std::string text = "quest";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, text);
}
std::unique_ptr<menus::menu> menus::menu_builder::build_inventory_menu(){
    std::string text = "inventory";
    Rectangle box = { 400 , 400 ,  200, 200};
    return std::make_unique<menus::menu>(box, text);    
}
menus::menu_graph menus::menu_builder::build_menus(){
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
    graph_.push_back(std::make_pair(build_node(m_builder_.build_blank_menu(), menu_ids::blank), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_pause_menu(), menu_ids::pause), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_tab_menu(), menu_ids::tab), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_inventory_menu(), menu_ids::inventory), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_map_menu(), menu_ids::map), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_shop_menu(), menu_ids::shop), std::vector<edge>{}));
    graph_.push_back(std::make_pair(build_node(m_builder_.build_quest_menu(), menu_ids::quest), std::vector<edge>{}));

    // Now add edges using pointers to nodes in the graph
    // Blank menu edges
    graph_[menu_ids::blank].second = {
        build_edge(&graph_[menu_ids::pause].first, controls_config::key_press_actions::back),
        build_edge(&graph_[menu_ids::tab].first, controls_config::key_press_actions::menu_open),
        build_edge(&graph_[menu_ids::shop].first, controls_config::key_press_actions::shop_open),
        build_edge(&graph_[menu_ids::quest].first, controls_config::key_press_actions::quests_open),
        build_edge(&graph_[menu_ids::inventory].first, controls_config::key_press_actions::inventory_open),
        build_edge(&graph_[menu_ids::map].first, controls_config::key_press_actions::map_open),
    };
    
    // Other menus all go back to blank
    graph_[menu_ids::pause].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::tab].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::shop].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::quest].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::inventory].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
    graph_[menu_ids::map].second = { build_edge(&graph_[menu_ids::blank].first, controls_config::key_press_actions::back) };
}

int menus::menu_graph::update(float delta){
    (void) delta;
    return 0;
}

void menus::menu_graph::on_key_press_event(const events::key_press& event){
    size_t key = event.get_key();
    // check the edges of current, if there is a key match, "move" 

    for(auto & edge : graph_[current_].second){
        if(edge.key_ == key){
            node* dst = edge.destination_menu_;
            current_ = dst->id_;
        }
    }
}

void menus::menu_graph::render(){
    graph_[current_].first.menu_->render();
}

