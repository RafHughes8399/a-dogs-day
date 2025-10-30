#include "level.h"
#include <iostream>

// ----------------------------------------- level graph ----------------------------------------- //
bool level::level_graph::find_path(Vector2 start, Vector2 end){
    (void) start;
    (void) end;

    // ? something along the lines of, 
    /**
     * node start = position_to_node(start)
     * node end= position_to_node(end)
     * 
     * then run a *
     */
} 
int level::level_graph::num_nodes(){
    return graph_.size();
}
int level::level_graph::num_edges(){
    return 0;
}
int level::level_graph::num_edges_from(node & node){
    (void) node;
    return 0;
}
level::level_graph::node& level::level_graph::position_to_node(Vector2 position){
    (void) position;
    // use the calculation from your notes
    auto n = node {}; //  ! care on the returning a referecne to a local variable, it will go out of scope 
                        // ! this is just placeholder so you should be fine  
    return n;
}
std::vector<level::level_graph::edge> level::level_graph::edges(){
    return {};
}
std::vector<level::level_graph::edge> level::level_graph::edges_from_node(node& node){
    (void) node;
    return {};
}
std::vector<level::level_graph::node> level::level_graph::nodes(){
    return {};
}
void level::level_graph::insert_node(Vector2 position){
    // node is built with a number and at position 
    graph_.push_back(std::make_pair(node{position}, std::vector<edge>{}));
    return;
}
void level::level_graph::insert_edge(int source_num, node& destination, int weight){
    graph_[source_num - 1 ].second.push_back(edge{std::make_shared<node>(destination), weight});

    return;
}
void level::level_graph::render(Rectangle frame){
    auto num_rendered = 0;
    for(auto x = frame.x; x <= frame.x + frame.width; x += dimensions_config::edge_weight){
        for(auto y = frame.y; y <= frame.y + frame.height; y += dimensions_config::edge_weight){

            // (row * row_length) + col
            int row = y / dimensions_config::edge_weight;
            int row_length = dimensions_config::world_x / dimensions_config::edge_weight;
            int col = x / dimensions_config::edge_weight;
            int index = (row * row_length) + col;
            //std::cout << "render node:  " << node_index <<std::endl;
            num_rendered++;
            auto position = graph_[index].first.position_;
            DrawCircle(position.x, position.y, 15, DARKGREEN);
            DrawText(TextFormat("%d", index), position.x, position.y, 32, WHITE);
        }
    }
    return;
}

// ----------------------------------------- level ----------------------------------------- //
void level::level::update(float delta){
    level_entities_.update(delta);
    return;
}
void level::level::render(){
    // draw the background 
    DrawTextureRec(background_.get_texture(), view_frame_, Vector2{0.0f, 0.0f}, WHITE);
    // draw the entities, based on the view frame
    graph_.render(view_frame_);
    auto bounds = raglib::bounding_box_2{Vector2{view_frame_.x, view_frame_.y}, 
    Vector2{view_frame_.x + view_frame_.width, view_frame_.y +view_frame_.height}};
    auto render_precdicate = [bounds](auto & entity) -> bool {
        return bounds.contains(entity->get_bounds());
    };
    level_entities_.render(render_precdicate);
    return;
}

void level::level::add_entity(std::unique_ptr<entities::entity> entity){
    level_entities_.insert(entity);
}

int level::level::entity_id(){
    return level_entities_.get_next_id();
}
int level::level::num_entities(){
    return level_entities_.size();
}

void level::level::on_left_mouse_event(const events::left_mouse_down& event){
    auto delta = event.get_mouse_delta();
    auto frame_delta = Vector2Scale(delta, -1); 
    // TODO update frame values and clamp them 
    // ahh because you're changing the position of the frame, so it should be 
    view_frame_.x = Clamp(view_frame_.x + frame_delta.x, 0.0, dimensions_config::world_x - GetScreenWidth());
    view_frame_.y = Clamp(view_frame_.y + frame_delta.y, 0.0, dimensions_config::world_y - GetScreenHeight());
}
void level::level::on_right_mouse_event(const events::right_mouse_click& event){

    auto click_position = event.get_mouse_position();
    auto paw = entities::e_builder.build_paw_mark(click_position, level_entities_.get_next_id());
    add_entity(std::move(paw));
}

// --------------------- level builder ----------------------------------------- //
level::level level::level_builder::build_main_level(){
    auto background = sprite::sprite(LoadTexture(assets_config::background_path), 
        assets_config::background_attributes[assets_config::attributes::frame_width], 
        assets_config::background_attributes[assets_config::attributes::frame_height],
        assets_config::background_attributes[assets_config::attributes::frames],
        assets_config::background_attributes[assets_config::attributes::animations]);
                
    auto view_frame = Rectangle{0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    auto dimensions = Vector2{dimensions_config::world_x, dimensions_config::world_y};

    auto l = level(background, view_frame, dimensions);

    // append the cursor
    auto cursor = entities::e_builder.build_cursor(GetMousePosition(), l.entity_id());
    l.add_entity(std::move(cursor));
    return l;
}