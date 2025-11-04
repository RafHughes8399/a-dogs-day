#include "level.h"

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
    return true;
} 

int level::level_graph::categorise_node(int row, int column){
    bool top_row = row == 0; // || row == max_row;
    bool bottom_row = row == num_rows_ - 1;
    bool first_column = column == 0;
    bool last_column = column == row_length_ - 1;
    bool first_or_last = column == 0; // || column == max_column;

    bool corner = ((top_row || bottom_row) && first_column) || ((top_row || bottom_row) && last_column);
    bool perimeter = (top_row || bottom_row) || (first_column || last_column);
    if(corner){return nodes::corner;}
    else if(perimeter) {return nodes::perimeter;}
    else {return nodes::interior;}
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

level::level_graph::node level::level_graph::position_to_node(Vector2 position){
    (void) position;
    // use the calculation from your notes
    auto n = node {}; //  ! care on the returning a referecne to a local variable, it will go out of scope 
                        // ! this is just placeholder so you should be fine  
    return n;
}

std::vector<level::level_graph::edge> level::level_graph::build_corner_edges(int row, int column){
    // check top or bottom, first or last
    bool top_row = row == 0;
    bool bottom_row = row == num_rows_ -1;
    bool left_column = column == 0;
    bool right_column = column == row_length_ - 1;
    std::vector<edge> edges = {};
    int source_index = (row * row_length_) + column; // index of the current node
    int destination_index = source_index;
    auto hypotenuse_weight = std::hypotf(dimensions_config::edge_weight, dimensions_config::edge_weight);

    if(top_row && left_column){ // top left corner
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_plus);
        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_plus);
        
        destination_index = source_index + row_length_ + 1;
        auto x_plus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_plus_y_plus);
    } 
    else if(top_row && ! left_column){ // top right corner
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_minus);
        
        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_minus_y_plus);
    }
    else if(! top_row && left_column){ // bottom left corner
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_minus);

        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_plus);
        
        destination_index = source_index - row_length_ + 1;
        auto x_plus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};      
        edges.push_back(x_plus_y_minus);  
    } 
    else if(! top_row && ! left_column){
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_minus);
        
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_minus);

        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_minus_y_minus);
    } // bottom right corner
    return edges;
}
std::vector<level::level_graph::edge> level::level_graph::build_interior_edges(int row, int column){
    std::vector<edge> edges = {};
    // index = (row * row_length) + column
    int source_index = (row * row_length_) + column; // index of the current node
    int destination_index = source_index;
    
    destination_index = source_index - row_length_;
    auto y_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
    edges.push_back(y_minus);

    destination_index =  source_index + row_length_;
    auto y_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
    edges.push_back(y_plus);
    
    destination_index = source_index - 1;
    auto x_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
    edges.push_back(x_minus);

    destination_index = source_index + 1;
    auto x_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
    edges.push_back(x_plus);
    
    auto hypotenuse_weight = std::hypotf(dimensions_config::edge_weight, dimensions_config::edge_weight);
    
    destination_index = source_index - row_length_ - 1;
    auto x_minus_y_minus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    edges.push_back(x_minus_y_minus);
    
    destination_index = source_index - row_length_ +  1;
    auto x_plus_y_minus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    edges.push_back(x_plus_y_minus);
    
    destination_index = source_index + row_length_ - 1;
    auto x_minus_y_plus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    edges.push_back(x_minus_y_plus);
    
    destination_index = source_index + row_length_ + 1;
    auto x_plus_y_plus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    edges.push_back(x_plus_y_plus);

    return edges;   
}
std::vector<level::level_graph::edge> level::level_graph::build_perimeter_edges(int row, int column){
    bool top_row = row == 0;
    bool bottom_row = row == num_rows_ -1;
    bool left_column = column == 0;
    bool right_column = column == row_length_ - 1;
    
    int source_index = (row * row_length_) + column;
    int destination_index = source_index;
    auto hypotenuse_weight = std::hypotf(dimensions_config::edge_weight, dimensions_config::edge_weight);
    
    std::vector<edge> edges = {};
    if(top_row){
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_minus);

        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_plus);

        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index + row_length_ - 1;
        auto x_minus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_minus_y_plus);
        
        destination_index = source_index + row_length_ + 1;
        auto x_plus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_plus_y_plus);
    }
    else if(bottom_row){
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_minus);

        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_plus);

        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_minus);
        
        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_minus_y_minus);
        
        destination_index = source_index - row_length_ + 1;
        auto x_plus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_plus_y_minus);
    }
    else if(left_column){
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_minus);
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_plus);
        
        destination_index = source_index - row_length_ + 1;
        auto x_plus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_plus_y_minus);

        destination_index = source_index + row_length_ + 1;
        auto x_plus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_plus_y_plus);
    }
    else if(right_column){
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_minus);
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index -1;
        auto x_minus = edge{&graph_[destination_index].first, dimensions_config::edge_weight};
        edges.push_back(x_minus);
        
        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_minus_y_minus);

        destination_index = source_index + row_length_ - 1;
        auto x_minus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        edges.push_back(x_minus_y_plus);
    }
    return edges;
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

void level::level_graph::build_edges(){
    // auto is a std::pair<node, std::vector<edge>>
    int num_edges = 0;
    for(auto & node : graph_){
        auto & n = node.first;
        auto & edges = node.second;
        // determine node, based on its row and its column
        int node_row = n.position_.y / dimensions_config::edge_weight;
        int node_column = n.position_.x / dimensions_config::edge_weight;
        // enum the node tpyes and do a switch
        auto node_type = categorise_node(node_row, node_column);
        auto node_edges = std::vector<edge>{};
        switch(node_type){
            case nodes::corner:
                // TODO something
                // ? exact outgoing edges depends on which corner (i.e top and bottom have different ones)
                node_edges = build_corner_edges(node_row, node_column);
                num_edges += 3;
                break; 
                case nodes::perimeter:
                node_edges = build_perimeter_edges(node_row, node_column);
                num_edges += 5;
                // TODO something
                break;
                case nodes::interior:
                // TODO something
                node_edges = build_interior_edges(node_row, node_column);
                num_edges += 8;
                break;
        }
        for(edge & e : node_edges){
            edges.push_back(e);
        }
    }
    std::cout << "edges created: " << num_edges << std::endl;
    return;
}
void level::level_graph::build_nodes(int level_x, int level_y){

    // goes across the row, and then down the column so you. a node can be found by
    // ! (row * row_length) + column
    int num_nodes = 0;
    for(int y = 0; y < level_y; y += dimensions_config::edge_weight){
        for(int x = 0; x < level_x; x += dimensions_config::edge_weight){
            Vector2 node_position = Vector2 {static_cast<float>(x),static_cast<float>(y)};
            num_nodes++;
            insert_node(node_position);
        }
    }
}
void level::level_graph::insert_node(Vector2 position){
    // node is built with a number and at position 
    graph_.push_back(std::make_pair(node{position}, std::vector<edge>{}));
    return;
}
void level::level_graph::insert_edge(int source_num, node& destination, float weight){
    return;
}
void level::level_graph::render(Rectangle frame){
    for(auto x = frame.x; x <= frame.x + frame.width; x += dimensions_config::edge_weight){
        for(auto y = frame.y; y <= frame.y + frame.height; y += dimensions_config::edge_weight){
            // (row * row_length) + col
            int row = y / dimensions_config::edge_weight;
            int row_length = dimensions_config::world_x / dimensions_config::edge_weight;
            int col = x / dimensions_config::edge_weight;
            int index = (row * row_length) + col;
            auto position = graph_[index].first.position_;
            DrawCircle(position.x, position.y, 15, DARKGREEN);
            DrawText(TextFormat("%d", index), position.x, position.y, 12, WHITE);
            
            auto edges = graph_[index].second;
            for(auto & e : edges){
                DrawLine(position.x, position.y, e.destination_->position_.x, e.destination_->position_.y, DARKGREEN);
            }
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
    auto bounds = raglib::bounding_box_2{Vector2{view_frame_.x, view_frame_.y}, 
    Vector2{view_frame_.x + view_frame_.width, view_frame_.y + view_frame_.height}};
    std::cout << "frame: " << view_frame_.x << ", " << view_frame_.y << ", " << view_frame_.x + view_frame_.width <<
    ", " << view_frame_.y + view_frame_.height << std::endl;
    auto render_precdicate = [bounds](auto & entity) -> bool { // auto is std::unique_ptr<entity>
        std::cout << "check bounds " << std::endl;
        bool b =  bounds.contains(entity->get_bounds());
        std::cout << "checked bounds " << std::endl;
        return b;
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

    view_frame_.x = Clamp(view_frame_.x + frame_delta.x, 0.0, dimensions_config::world_x - GetScreenWidth());
    view_frame_.y = Clamp(view_frame_.y + frame_delta.y, 0.0, dimensions_config::world_y - GetScreenHeight());
    std::cout << "level left click event " << std::endl;
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