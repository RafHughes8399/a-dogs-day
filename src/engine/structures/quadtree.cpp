#include "quadtree.h"
#include "component.h"
#include "raglib.h"
#include <raylib.h>
enum positions{
    top_right = 0,
    top_left = 1,
    bottom_left = 2,
    bottom_right = 3
};

// ----------------------- ecs_quadtree ----------------------- //

// ---------------- ecs_quadtree - node geometry ----------------
hitbox::hitbox* tree::ecs_quadtree::bounds_for(size_t entity_id){
    auto* collision = component_managers::collision_manager_.get_component(entity_id);
    if(collision == nullptr){ return nullptr; }
    return &collision->get_hitbox_component().get_hitbox();
}
std::optional<Rectangle> tree::ecs_quadtree::interactor_bounds_for(size_t entity_id){
    auto* interactor = component_managers::interactor_manager_.get_component(entity_id);
    auto hitbox = bounds_for(entity_id);

    if(not hitbox or not interactor) {return std::nullopt; }
    return interactor->get_interaction_box(hitbox->get_box());
}

bool tree::ecs_quadtree::node_contains_object(raglib::bounding_box_2& node, const Rectangle& object){
    return node.contains(object);
}
bool tree::ecs_quadtree::node_contains_position(raglib::bounding_box_2& node, const Vector2& position){
    return node.contains(position);
}

bool tree::ecs_quadtree::node_overlaps_object(raglib::bounding_box_2& node, const Rectangle& object){
    auto node_rect = Rectangle{node.min.x, node.min.y,
        node.max.x - node.min.x, node.max.y - node.min.y};
    return CheckCollisionRecs(object, node_rect);
}

int tree::ecs_quadtree::object_contained_by_child(raglib::bounding_box_2& node, Rectangle& object){
    auto centre = Vector2Add(node.max, node.min);
    centre = Vector2Scale(centre, 0.5f);

    Vector2 object_min = Vector2{object.x, object.y};
    Vector2 object_max = Vector2{object.x + object.width, object.y + object.height};
    bool crosses_centre = (object_min.x < centre.x and centre.x < object_max.x)
    or (object_min.y < centre.y and centre.y < object_max.y);

    if(crosses_centre) {return -1;}

    auto children = std::vector<raglib::bounding_box_2>{};
    children.push_back(raglib::bounding_box_2{centre, node.max});
    children.push_back(raglib::bounding_box_2{Vector2{node.min.x, centre.y}, Vector2{centre.x, node.max.y}});
    children.push_back(raglib::bounding_box_2{node.min, centre});
    children.push_back(raglib::bounding_box_2{Vector2{centre.x, node.min.y},  Vector2{node.max.x, centre.y}});
    for(size_t i = 0; i < CHILDREN; ++i){
        auto child_node = children.at(i);
        if(node_contains_object(child_node, object)){
            return int(i);
        }
    }
    return -1;
}

bool tree::ecs_quadtree::is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child){
    for(auto& c : tree->children_){
        if(*c == *child){return true;}
    }
    return false;
}

void tree::ecs_quadtree::build_children(std::unique_ptr<node>& tree){
    auto centre = Vector2Add(tree->bounds_.max, tree->bounds_.min);
    centre = Vector2Scale(centre, 0.5f);

    auto top_right = std::make_unique<node>();
    top_right->depth_ = tree->depth_ + 1;
    top_right->life_ = 0;
    top_right->bounds_ =  raglib::bounding_box_2{centre, tree->bounds_.max};
    tree->children_.push_back(std::move(top_right));

    auto top_left = std::make_unique<node>();
    top_left->depth_ = tree->depth_ + 1;
    top_left->life_ = 0;
    top_left->bounds_ = raglib::bounding_box_2{Vector2{tree->bounds_.min.x, centre.y},  Vector2{centre.x, tree->bounds_.max.y}};
    tree->children_.push_back(std::move(top_left));

    auto bottom_left = std::make_unique<node>();
    bottom_left->depth_ = tree->depth_ + 1;
    bottom_left->life_ = 0;
    bottom_left->bounds_ = raglib::bounding_box_2{tree->bounds_.min, centre};
    tree->children_.push_back(std::move(bottom_left));

    auto bottom_right = std::make_unique<node>();
    bottom_right->depth_ = tree->depth_ + 1;
    bottom_right->life_ = 0;
    bottom_right->bounds_ = raglib::bounding_box_2{Vector2{centre.x, tree->bounds_.min.y},  Vector2{tree->bounds_.max.x, centre.y}};
    tree->children_.push_back(std::move(bottom_right));
}

// ---------------- ecs_quadtree - insertion and removal ----------------
void tree::ecs_quadtree::insert(std::unique_ptr<node>& tree, size_t entity_id, hitbox::hitbox& bounds){
    if(not node_contains_object(tree->bounds_, bounds.get_box())){
        return;
    }
    else{
        if(tree->depth_ == max_depth_){
            tree->entities_.push_back(entity_id);
            return;
        }
        else if(is_leaf(tree)){
            build_children(tree);
        }
        for (auto& child : tree->children_) {
            if (node_contains_object(child->bounds_, bounds.get_box())) {
                insert(child, entity_id, bounds);
                return;
            }
        }
        tree->entities_.push_back(entity_id);
    }
}

void tree::ecs_quadtree::erase(std::unique_ptr<node>& tree, size_t entity_id){
    if(not tree){
        return;
    }
    auto new_end = std::remove(tree->entities_.begin(), tree->entities_.end(), entity_id);
    if(new_end == tree->entities_.end()){
        for(auto& child : tree->children_){
            erase(child, entity_id);
        }
    }
    else{
        tree->entities_.erase(new_end, tree->entities_.end());
        return;
    }
}

bool tree::ecs_quadtree::contains(std::unique_ptr<node>& tree, size_t entity_id){
    if(not tree){ return false; }
    if(std::find(tree->entities_.begin(), tree->entities_.end(), entity_id) != tree->entities_.end()){
        return true;
    }
    for(auto& child : tree->children_){
        if(contains(child, entity_id)){ return true; }
    }
    return false;
}

void tree::ecs_quadtree::clear(std::unique_ptr<node>& tree){
    tree->entities_.clear();
    for(auto& child : tree->children_){
        clear(child);
    }
}

// ---------------- ecs_quadtree - inspection ----------------
int tree::ecs_quadtree::height(std::unique_ptr<node>& tree) {
    if (not tree) {
        return -1;
    }
    else {
        int max_child_height = -1;
        for (auto& child : tree->children_) {
            int child_height = height(child);
            if (child_height >= max_child_height) {
                max_child_height = child_height;
            }
        }
        return 1 + max_child_height;
    }
}

size_t tree::ecs_quadtree::size(std::unique_ptr<node>& tree) {
    auto empty = is_empty(tree);
    if (empty) { return 0; }
    if(not tree){
        return 0;
    }
    else {
        auto t_size = tree->entities_.size();
        for (auto& child : tree->children_) {
            t_size += size(child);
        }
        return t_size;
    }
    return 0;
}

size_t tree::ecs_quadtree::num_nodes(std::unique_ptr<node>& tree){
    if(tree){
        size_t size = 1;
        for(auto& child : tree->children_){
            size += num_nodes(child);
        }
        return size;
    }
    else{
        return 0;
    }
}

bool tree::ecs_quadtree::is_empty(std::unique_ptr<node>& tree) {
    auto empty = tree->entities_.size() == 0 ? true : false;
    if (not empty) { return false; }
    for (auto& child : tree->children_) {
        if (not is_empty(child)) {
            return false;
        }
    }
    return true;
}

bool tree::ecs_quadtree::is_root(std::unique_ptr<node>& tree){
    return tree->depth_ == 0  ? true : false;
}

bool tree::ecs_quadtree::is_leaf(std::unique_ptr<node>& tree) {
    return tree->children_.size() == 0 ? true : false;
}

// ---------------- ecs_quadtree - collision ----------------
// the querying entity is skipped - the cursor sits on its own click position
// every frame, so without this a click never sees past it
int tree::ecs_quadtree::is_there_collision(std::unique_ptr<node>& tree, Vector2 position, size_t id,
    const std::function<bool(size_t)>& filter){
    if(not tree) {return game_config::empty_entity;}
    for(size_t entity : tree->entities_){
        if(entity == id){ continue; }
        if(not filter(entity)){ continue; }
        auto entity_collision_component = component_managers::collision_manager_.get_component(entity);
        if(entity_collision_component == nullptr){ continue; }
        auto entity_bounds = entity_collision_component->get_hitbox_component().get_hitbox().get_box();
        if(CheckCollisionPointRec(position, entity_bounds)){
            return static_cast<int>(entity);
        }
    }
    // contains() is inclusive on both bounds, so a position on a split belongs to
    // more than one child - carry on through the siblings rather than committing
    // to the first
    for(auto& child : tree->children_){
        if(node_contains_position(child->bounds_, position)){
            int entity = is_there_collision(child, position, id, filter);
            if(entity != game_config::empty_entity){
                return entity;
            }
        }
    }
    return game_config::empty_entity;
}
int tree::ecs_quadtree::is_there_collision(std::unique_ptr<node>& tree, Rectangle box, size_t id,
    const std::function<bool(size_t)>& filter){
    if(not tree) {return game_config::empty_entity;}
    for(size_t entity : tree->entities_){
        if(entity == id){ continue; }
        if(not filter(entity)){ continue; }
        auto entity_collision_component = component_managers::collision_manager_.get_component(entity);
        if(entity_collision_component == nullptr){ continue; }
        auto entity_bounds = entity_collision_component->get_hitbox_component().get_hitbox().get_box();
        if(CheckCollisionRecs(box, entity_bounds)){
            return static_cast<int>(entity);
        }
    }
    // node_contains_object demands full containment, so a box straddling a split
    // matches no child at all and the descent stops here. a miss is not an
    // acceptable broad-phase answer, so overlap - not containment - decides which
    // children are worth searching
    for(auto& child : tree->children_){
        if(node_overlaps_object(child->bounds_, box)){
            int entity = is_there_collision(child, box, id, filter);
            if(entity != game_config::empty_entity){
                return entity;
            }
        }
    }
    return game_config::empty_entity;
}
int tree::ecs_quadtree::is_there_interaction(std::unique_ptr<node>& tree, Rectangle box, size_t id){
    if(not tree){
        return game_config::empty_entity;
    }
    for(auto entity_id : tree->entities_){
        if(id == entity_id){ continue; }
        auto other_box_opt = interactor_bounds_for(entity_id);
        if(other_box_opt.has_value()){
            if(CheckCollisionRecs(box, other_box_opt.value())){
                return static_cast<int>(entity_id);
            }
        }
    }
    for(auto & child : tree->children_){
        if(node_overlaps_object(child->bounds_, box)){
            int entity = is_there_interaction(child, box, id);
            if(entity != game_config::empty_entity) {return entity;}
        }
    }
    return game_config::empty_entity;
}
bool tree::ecs_quadtree::get_colliding_entity(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, size_t id, size_t& found){
    if(not tree) {
        return false;
    }
    for(auto entity_id : tree->entities_){
        if(id == entity_id){ continue; }
        auto* other = bounds_for(entity_id);
        if(other != nullptr and bounds.check_collision(*other)){
            found = entity_id;
            return true;
        }
    }
    for(auto & child : tree->children_){
        if(node_contains_object(child->bounds_, bounds.get_box())){
            if(get_colliding_entity(child, bounds, id, found)){
                return true;
            }
        }
    }
    return false;
}

// ---------------- ecs_quadtree - maintenance ----------------
void tree::ecs_quadtree::prune_leaves(std::unique_ptr<node>& tree, double delta) {
    if (is_leaf(tree) and not is_root(tree)
        and is_empty(tree)) {
        tree->life_ += short(delta);
        if (tree->life_ >= NODE_LIFETIME) {
            tree.reset();
            return;
        }
    }
    else {
        tree->life_ = 0;
        for (auto& child : tree->children_) {
            prune_leaves(child, delta);
        }
        auto new_end = std::remove_if(tree->children_.begin(), tree->children_.end(),
        [](auto& child) -> bool {
            return not child;
        });
        tree->children_.erase(new_end, tree->children_.end());
    }
    return;
}

std::unique_ptr<tree::ecs_quadtree::node> tree::ecs_quadtree::copy_tree(node* tree, std::unique_ptr<node>* parent){
    (void) parent;
    if(not tree){
        return nullptr;
    }
    auto copy = std::make_unique<node>();
    copy->bounds_ = tree->bounds_;
    copy->depth_ = tree->depth_;
    copy->life_ = tree->life_;
    copy->entities_ = tree->entities_;
    auto new_parent = &copy;
    for(auto & child : tree->children_){
        copy->children_.push_back(copy_tree(child.get(), new_parent));
    }
    return copy;
}

void tree::ecs_quadtree::traverse_tree(std::unique_ptr<node>& tree){
    if(not tree){ return; }
    for(auto& child : tree->children_){
        traverse_tree(child);
    }
}

int tree::ecs_quadtree::depth_of(std::unique_ptr<node>& tree, size_t entity_id){
    if(not tree){ return -1; }
    if(std::find(tree->entities_.begin(), tree->entities_.end(), entity_id) != tree->entities_.end()){
        return tree->depth_;
    }
    for(auto& child : tree->children_){
        auto found = depth_of(child, entity_id);
        if(found != -1){ return found; }
    }
    return -1;
}

bool tree::ecs_quadtree::node_bounds_of(std::unique_ptr<node>& tree, size_t entity_id, raglib::bounding_box_2& bounds){
    if(not tree){ return false; }
    if(std::find(tree->entities_.begin(), tree->entities_.end(), entity_id) != tree->entities_.end()){
        bounds = tree->bounds_;
        return true;
    }
    for(auto& child : tree->children_){
        if(node_bounds_of(child, entity_id, bounds)){ return true; }
    }
    return false;
}
