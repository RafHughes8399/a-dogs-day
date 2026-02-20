#include "level.h"

// ----------------------------------------- level graph ----------------------------------------- //
bool level::level_graph::can_place_decoration(const queries::can_place_decoration& query){
    auto query_rectangle = query.get_decoration_rectangle();
    return  ! check_for_decoration(query_rectangle);
}
bool level::level_graph::is_node_closer(int current_id, int next_id, int end_id){
    Vector2 current_position = id_to_node(current_id)->position_;
    Vector2 next_position = id_to_node(next_id)->position_;
    Vector2 end_position = id_to_node(end_id)->position_;

    auto current_end_distance = Vector2Distance(current_position, end_position);
    auto next_end_distance = Vector2Distance(next_position, end_position);
    return next_end_distance <= current_end_distance;

}
bool level::level_graph::is_node_occupied(int id){
    return id_to_node(id)->decoration_ != -1; 
}


level::level_graph::node* level::level_graph::id_to_node(int id){
    // is it just as simple as 
    return &graph_[id].first;
}
std::vector<int> level::level_graph::bfs(int start_id, int end_id){
    size_t visited_size = graph_.size();
    auto visited = std::vector<int>(visited_size, -1);
    // init all with -1 
    bool found = false;
    
    visited[start_id] = start_id;
    
    auto nodes = std::queue<int>();
    nodes.push(start_id);
    while(! nodes.empty() && ! found){
        auto current = nodes.front();
        nodes.pop();
        if(current == end_id){
            found = true;
            return visited;
        }
        else{
            // for each outgoing edge from current 
            for(auto & edge : graph_[current].second){
                // if not visited, and closer to the end 
                // TODO and not "occupied", like not blocked by a decoration
                /**
                 * ? while i like the idea of not looking at nodes going in the wrong direction, i feel 
                 * ? that it may cause errors down the line when many decorations are in the map and a direct, "always moving 
                 * ? closer" path may not exist
                 */
                auto closer = is_node_closer(current, edge.destination_->id_, end_id);
                auto occupied = is_node_occupied(edge.destination_->id_);
                // then explore
                if(visited[edge.destination_->id_] == -1 && ! occupied){
                    visited[edge.destination_->id_] = current;
                    nodes.push(edge.destination_->id_);
                }
            }
        }
    }
    return visited;
}
std::vector<Vector2> level::level_graph::make_position_path(std::vector<Vector2>& position_path, std::vector<int>& visited, int start_id, int end_id){
    auto path = std::vector<Vector2>{};
    int current_id = end_id;
    while(current_id != start_id){
        path.push_back(graph_[current_id].first.position_);
        current_id = visited[current_id];
    }
    path.push_back(graph_[start_id].first.position_);
    std::reverse(path.begin(), path.end());
    for(auto & position : path){
        position_path.push_back(position);
    }

    return path;
}
std::vector<Vector2> level::level_graph::find_path(Vector2 start, Vector2 end, Vector2 direction){
    // TODO Implement !
    // ? something along the lines of, 
    /**
     * node start = position_to_node(start)
     * node end= position_to_node(end)
     * 
     * then run a *
     */
    /** this should be pointers */
    int start_node = position_to_node(start, direction);
    int end_node = position_to_node(end, direction);
    
    auto node_path = bfs(start_node, end_node);
    // convert node path to position path
    
    auto position_path = std::vector<Vector2>();

    make_position_path(position_path, node_path, start_node, end_node);

    return position_path;

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

// similar to the function below, but assumes that the position is 
// a clean multiple of edge weight
int level::level_graph::position_to_node(Vector2 position){
    int row = position.y / level_config::edge_weight;
    int col= position.x / level_config::edge_weight;

    return (row * row_length_) + col;
}
// snaps to the nearest node based on the direction being travelled 
// assumes that the position is not a clean multiple of level_config::edge_weight
int level::level_graph::position_to_node(Vector2 position, Vector2 direction){
    
    float row_f = position.y / level_config::edge_weight;
    float column_f = position.x / level_config::edge_weight; 
    int row, column;
    
    // Always snap forward in direction of travel
    if(direction.x >= 0) {
        column = static_cast<int>(std::ceil(column_f));
    }
        
    else{
        column = static_cast<int>(std::floor(column_f));
    } 
        
    
    if(direction.y >= 0) {
      row = static_cast<int>(std::ceil(row_f));
    } 
    
    else{
        row = static_cast<int>(std::floor(row_f));
    } 
    
    // clamp
    column = std::max(0, std::min(column, row_length_ - 1));
    row = std::max(0, std::min(row, num_rows_ - 1));
    
    int index = (row * row_length_) + column;

    return index;
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
    auto hypotenuse_weight = std::hypotf(level_config::edge_weight, level_config::edge_weight);

    if(top_row && left_column){ // top left corner
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_plus);
        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_plus);
        
        destination_index = source_index + row_length_ + 1;
        auto x_plus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_plus_y_plus);
    } 
    else if(top_row && ! left_column){ // top right corner
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_minus);
        
        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_minus_y_plus);
    }
    else if(! top_row && left_column){ // bottom left corner
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_minus);

        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_plus);
        
        destination_index = source_index - row_length_ + 1;
        auto x_plus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};      
        // edges.push_back(x_plus_y_minus);  
    } 
    else if(! top_row && ! left_column){
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_minus);
        
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_minus);

        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_minus_y_minus);
    } // bottom right corner
    return edges;
}
std::vector<level::level_graph::edge> level::level_graph::build_interior_edges(int row, int column){
    std::vector<edge> edges = {};
    // index = (row * row_length) + column
    int source_index = (row * row_length_) + column; // index of the current node
    int destination_index = source_index;
    
    destination_index = source_index - row_length_;
    auto y_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
    edges.push_back(y_minus);

    destination_index =  source_index + row_length_;
    auto y_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
    edges.push_back(y_plus);
    
    destination_index = source_index - 1;
    auto x_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
    edges.push_back(x_minus);

    destination_index = source_index + 1;
    auto x_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
    edges.push_back(x_plus);
    
    auto hypotenuse_weight = std::hypotf(level_config::edge_weight, level_config::edge_weight);
    
    destination_index = source_index - row_length_ - 1;
    auto x_minus_y_minus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    // edges.push_back(x_minus_y_minus);
    
    destination_index = source_index - row_length_ +  1;
    auto x_plus_y_minus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    // edges.push_back(x_plus_y_minus);
    
    destination_index = source_index + row_length_ - 1;
    auto x_minus_y_plus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    // edges.push_back(x_minus_y_plus);
    
    destination_index = source_index + row_length_ + 1;
    auto x_plus_y_plus =  edge{&graph_[destination_index].first, hypotenuse_weight};
    // edges.push_back(x_plus_y_plus);

    return edges;   
}
std::vector<level::level_graph::edge> level::level_graph::build_perimeter_edges(int row, int column){
    bool top_row = row == 0;
    bool bottom_row = row == num_rows_ -1;
    bool left_column = column == 0;
    bool right_column = column == row_length_ - 1;
    
    int source_index = (row * row_length_) + column;
    int destination_index = source_index;
    auto hypotenuse_weight = std::hypotf(level_config::edge_weight, level_config::edge_weight);
    
    std::vector<edge> edges = {};
    if(top_row){
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_minus);

        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_plus);

        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index + row_length_ - 1;
        auto x_minus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_minus_y_plus);
        
        destination_index = source_index + row_length_ + 1;
        auto x_plus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_plus_y_plus);
    }
    else if(bottom_row){
        destination_index = source_index - 1;
        auto x_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_minus);

        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_plus);

        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_minus);
        
        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_minus_y_minus);
        
        destination_index = source_index - row_length_ + 1;
        auto x_plus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_plus_y_minus);
    }
    else if(left_column){
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_minus);
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index + 1;
        auto x_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_plus);
        
        destination_index = source_index - row_length_ + 1;
        auto x_plus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_plus_y_minus);

        destination_index = source_index + row_length_ + 1;
        auto x_plus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_plus_y_plus);
    }
    else if(right_column){
        destination_index = source_index - row_length_;
        auto y_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_minus);
        destination_index = source_index + row_length_;
        auto y_plus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(y_plus);
        
        destination_index = source_index -1;
        auto x_minus = edge{&graph_[destination_index].first, level_config::edge_weight};
        edges.push_back(x_minus);
        
        destination_index = source_index - row_length_ - 1;
        auto x_minus_y_minus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_minus_y_minus);

        destination_index = source_index + row_length_ - 1;
        auto x_minus_y_plus = edge{&graph_[destination_index].first, hypotenuse_weight};
        // edges.push_back(x_minus_y_plus);
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
    for(auto & node : graph_){
        auto & n = node.first;
        auto & edges = node.second;
        // determine node, based on its row and its column
        int node_row = n.position_.y / level_config::edge_weight;
        int node_column = n.position_.x / level_config::edge_weight;
        // enum the node tpyes and do a switch
        auto node_type = categorise_node(node_row, node_column);
        auto node_edges = std::vector<edge>{};
        switch(node_type){
            case nodes::corner:
                // ? exact outgoing edges depends on which corner (i.e top and bottom have different ones)
                node_edges = build_corner_edges(node_row, node_column);
                break; 
                case nodes::perimeter:
                node_edges = build_perimeter_edges(node_row, node_column);
                break;
                case nodes::interior:
                node_edges = build_interior_edges(node_row, node_column);
                break;
        }
        for(edge & e : node_edges){
            edges.push_back(e);
        }
    }
    return;
}
void level::level_graph::build_nodes(int level_x, int level_y){
    // goes across the row, and then down the column so you. a node can be found by
    // ! (row * row_length) + column
    int num_nodes = 0;
    for(int y = 0; y < level_y; y += level_config::edge_weight){
        for(int x = 0; x < level_x; x += level_config::edge_weight){
            Vector2 node_position = Vector2 {static_cast<float>(x),static_cast<float>(y)};
            insert_node(num_nodes, node_position);
            num_nodes++;
        }
    }
}
void level::level_graph::insert_node(int id, Vector2 position){
    // node is built with a number and at position 
    graph_.push_back(std::make_pair(node{id, position, level_config::empty_node}, std::vector<edge>{}));
    return;
}
void level::level_graph::insert_edge(int source_num, node& destination, float weight){
    return;
}
bool level::level_graph::check_for_decoration(Rectangle rectangle){
    for(auto col = rectangle.x; col <= rectangle.x + rectangle.width; col += level_config::edge_weight){
        for(auto row = rectangle.y; row <= rectangle.y + rectangle.height; row += level_config::edge_weight){
            auto position = Vector2{col, row};
            int node_index = position_to_node(position);
            if(is_node_occupied(node_index)){
                return true;
            }
        }
    }
    return false;
}
void level::level_graph::update_decoration(Rectangle rectangle, int id){
    // start at the position x,y and iterate in increments of edge weight until 
    int nodes_affected = 0;
    for(auto col = rectangle.x; col <= rectangle.x + rectangle.width; col += level_config::edge_weight){
        for(auto row = rectangle.y; row <= rectangle.y + rectangle.height; row += level_config::edge_weight){
            auto position = Vector2{col, row};
            int node_index = position_to_node(position);

            graph_[node_index].first.decoration_ = id;
            nodes_affected++;
        }
    }
    std::cout << "updated nodes : " << nodes_affected << std::endl;
}
// for these two functions the following assumptions are
// the decoration is placed at positions that are multiples of edge weights
// and so too are their dimensions (width and height)
void level::level_graph::on_moved_decoration(const events::moved_decoration& event){
    std::cout << "move decoration in graph representation" << std::endl;
    update_decoration(event.get_pre_move());
    update_decoration(event.get_post_move(), event.get_id());
}
void level::level_graph::on_placed_decoration(const events::placed_decoration& event){
    std::cout << "place decoration in graph representation" << std::endl;
    update_decoration(event.get_rectangle(), event.get_id());
}
void level::level_graph::render(Rectangle frame){
    for(auto x = frame.x; x <= frame.x + frame.width; x += level_config::edge_weight){
        for(auto y = frame.y; y <= frame.y + frame.height; y += level_config::edge_weight){
            // (row * row_length) + col
            int row = y / level_config::edge_weight;
            int row_length = level_config::world_x / level_config::edge_weight;
            int col = x / level_config::edge_weight;
            int index = (row * row_length) + col;
            auto position = graph_[index].first.position_;
            if(graph_[index].first.decoration_ == level_config::empty_node){
                DrawCircle(position.x, position.y, 15, DARKGREEN);
            }
            else{
                DrawCircle(position.x, position.y, 15, RED);
            }
            DrawText(TextFormat("%d", graph_[index].first.id_), position.x, position.y, 12, WHITE);
            
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
    auto to_remove = level_entities_.update(delta); // could return a list of entiteis to remove ? 

    // only if there are entities to remove
    if(! to_remove.empty()){

        for(auto & layer : render_layers_){
            layer.remove_entities(to_remove);
        }
    }
    return;
}
void level::level::render(){
    // draw the background 
    DrawTextureRec(background_.get_texture(), view_frame_, Vector2{0.0f, 0.0f}, WHITE);
    // for debugging purposes
    graph_.render(view_frame_);


    // draw the entities, based on the view frame
    auto render_precdicate = [this](entities::entity*& entity) -> bool { // auto is std::unique_ptr<entity>
        const Rectangle& entity_box = entity->get_hitbox().get_box();
        auto position = entity->get_position();

        return view_frame_.x <= entity_box.x && view_frame_.y <= entity_box.y 
        && (view_frame_.x + view_frame_.width) >= (entity_box.x + entity_box.width) 
        && (view_frame_.y + view_frame_.height) >= (entity_box.y + entity_box.height);

    };

    for(size_t i = 0; i < level_config::draw_layers::size; ++i){
        render_layers_[i].draw(render_precdicate, Vector2{view_frame_.x, view_frame_.y});
    }
    return;
}

void level::level::add_entity(std::unique_ptr<entities::entity> entity, size_t layer){
    // make raw pointer and insert ot 
    auto entity_raw = entity.get();
    level_entities_.insert(std::move(entity));
    // insert into the draw layer ?
    render_layers_[layer].add_entity(entity_raw);
    id_entity_map_[entity_raw->get_id()] = entity_raw;
}

int level::level::entity_id(){
    return level_entities_.get_next_id();
}
int level::level::num_entities(){
    return level_entities_.size();
}

void level::level::on_left_mouse_click_event(const events::left_mouse_click& event){
    (void) event;

    return;
}
void level::level::on_move_view_frame_event(const events::move_view_frame& event){
    auto delta = event.get_delta();

    view_frame_.x = Clamp(view_frame_.x + delta.x, 0.0, level_config::world_x - GetScreenWidth());
    view_frame_.y = Clamp(view_frame_.y + delta .y, 0.0, level_config::world_y - GetScreenHeight());

    
}
void level::level::on_right_mouse_event(const events::right_mouse_click& event){

    auto click_position = event.get_mouse_position();
    auto paw = entities::e_builder.build_paw_mark(click_position, level_entities_.get_next_id());
    add_entity(std::move(paw), level_config::draw_layers::hud);

    auto dog_id = event.get_selected_dog();
    if(dog_id != -1){
        auto dog = id_entity_map_[dog_id];
        // add the direction too
        // cast and set the path
        auto dog_cast = static_cast<entities::player_dog*>(dog);
        auto direction = dog_cast->get_direction_scalar();
        auto dog_path = graph_.find_path(dog->get_position(), click_position, direction);
        dog_cast->set_path(dog_path);
    }
}

// --------------------- level builder ----------------------------------------- //
level::level level::level_builder::build_main_level(){
    auto background = sprite::sprite(LoadTexture(entity_config::background_path), 
        entity_config::background_attributes[entity_config::attributes::frame_width], 
        entity_config::background_attributes[entity_config::attributes::frame_height],
        entity_config::background_attributes[entity_config::attributes::frames],
        entity_config::background_attributes[entity_config::attributes::animations]);
                
    auto view_frame = Rectangle{0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    auto dimensions = Vector2{level_config::world_x, level_config::world_y};

    auto l = level(background, view_frame, dimensions);
    

    // 0 
    auto mack = entities::e_builder.build_mack(Vector2 {level_config::edge_weight * 7, level_config::edge_weight * 4}, l.entity_id());
    l.add_entity(std::move(mack), level_config::draw_layers::dogs);


    // 1
    auto khiri = entities::e_builder.build_khiri(Vector2 {level_config::edge_weight * 4, static_cast<float>(level_config::edge_weight * 3.5)}, l.entity_id());
    l.add_entity(std::move(khiri), level_config::draw_layers::dogs);

    // 2
    //  append the cursor
    auto cursor = entities::e_builder.build_cursor(GetMousePosition(), l.entity_id());
    l.add_entity(std::move(cursor), level_config::draw_layers::cursor);
    
    auto test_decoration = entities::e_builder.build_test_decoration(Vector2 {level_config::edge_weight * 6, level_config::edge_weight * 6}, l.entity_id());
    l.add_entity(std::move(test_decoration), level_config::draw_layers::decoration);
    
    auto second_decoration = entities::e_builder.build_test_decoration(Vector2 {level_config::edge_weight * 12, level_config::edge_weight * 12}, l.entity_id());
    l.add_entity(std::move(second_decoration), level_config::draw_layers::decoration);
    return l;
}