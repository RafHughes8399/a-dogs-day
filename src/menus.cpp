#include "menus.h"

// define the builder 
menus::menu_builder menus::m_builder_;


// ------------------------------- menu --------------------------------------- //
void menus::menu::render(){
    // TODO
    return;
}
// ------------------------------ builder ---------------------------------- // 


// ------------------------------ graph --------------------------------------- // 
menus::menu_graph::edge menus::menu_graph::build_edge(node& src, node& dst, int key){
    (void) src;
    (void) dst;
    (void) key;
    return edge{};
}
menus::menu_graph::node menus::menu_graph::build_node(std::unique_ptr<menu> & menu, size_t id){
    (void) menu;
    (void) id;
    return node {};
}
void menus::menu_graph::build_graph(){
    return;
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
            auto dst = edge.destination_menu_;
            current_ = dst->id_;
        }
    }
}

void menus::menu_graph::render(){
    graph_[current_].first.menu_->render();
}

