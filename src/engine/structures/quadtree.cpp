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

// ---------------- quadtree - node geometry ----------------
// containment checks
bool tree::quadtree::node_contains_object(raglib::bounding_box_2& node, const Rectangle& object){
    // compare the bounding box of the node and the object
    return node.contains(object);
}

// return the child "index" that the object can fit into, if -1 then no child can fit the object
int tree::quadtree::object_contained_by_child(raglib::bounding_box_2& node, Rectangle& object){
    // check if the object will fit into potential children of the node 
    auto centre = Vector2Add(node.max, node.min);
    centre = Vector2Scale(centre, 0.5f);

    Vector2 object_min = Vector2{object.x, object.y};
    Vector2 object_max = Vector2{object.x + object.width, object.y + object.height};
    // first check if the object crosses the centre of any axis, if it does then no child will fit it
    bool crosses_centre = (object_min.x < centre.x and centre.x < object_max.x) 
    or (object_min.y < centre.y and centre.y < object_max.y);
    
    if(crosses_centre) {return -1;}

    // setup the bounding boxes for the potential children
    auto children = std::vector<raglib::bounding_box_2>{};
    // 0
    children.push_back(raglib::bounding_box_2{centre, node.max});
    // 1
    children.push_back(raglib::bounding_box_2{Vector2{node.min.x, centre.y}, Vector2{centre.x, node.max.y}});
    // 2
    children.push_back(raglib::bounding_box_2{node.min, centre});
    // 3
    children.push_back(raglib::bounding_box_2{Vector2{centre.x, node.min.y},  Vector2{node.max.x, centre.y}});
    // otherwise, check which child will fit the object
    for(size_t i = 0; i < CHILDREN; ++i){
        // check the child that will contain the object
        auto child_node = children.at(i);
        if(node_contains_object(child_node, object)){
            // return the "index" of the child that will fit the node
            return int(i);
        }
    }
    return -1;
}
// child construction 
bool tree::quadtree::is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child){
    for(auto& c : tree->children_){
        if(*c == *child){return true;}
    }
    return false;
}

void tree::quadtree::build_children(std::unique_ptr<node>& tree){
    auto centre = Vector2Add(tree->bounds_.max, tree->bounds_.min);
    centre = Vector2Scale(centre, 0.5f);

    // create the child
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

// ---------------- quadtree - insertion and removal ----------------
// insertion
void tree::quadtree::insert(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity> object){
	auto & object_bounds = object->get_hitbox();
    // check if the node contains the object, if not then immediately return
	if(not node_contains_object(tree->bounds_, object_bounds.get_box())){ 
        return; 
    }
    else{
        // if at the max depth then insert, no further children can be constructed
        if(tree->depth_ == max_depth_){
            tree->objects_.push_back(std::move(object));
            return;
		}
        else if(is_leaf(tree)){
            build_children(tree);
        }
        // if not all children for the tree have been built
        // recursively iterate through the children
        
	    for (auto& child : tree->children_) {
            // if does fit in a child, recursively insert
            if (node_contains_object(child->bounds_, object_bounds.get_box())) {
                insert(child, std::move(object));
                return;
            }
		}
        // if this point is reached, there are no children that the object can fit into so insert into the node
        tree->objects_.push_back(std::move(object));
	}
}

void tree::quadtree::erase(std::unique_ptr<node>& tree, size_t object_id){
    if(not tree){
        return;
    } 
    auto new_end = std::remove_if(tree->objects_.begin(), tree->objects_.end(),
        [object_id](auto& obj) -> bool{
            if(object_id == static_cast<size_t>(obj->get_id())){
                return true;
            }
            return false;
        });
    // if nothing is to be erased, then check the children
    if(new_end == tree->objects_.end()){
        for(auto& child : tree->children_){
            erase(child, object_id);
        }

    }
    // an object is to be removed
    else{
        tree->objects_.erase(new_end, tree->objects_.end());
        return;
    }
}

std::unique_ptr<entities::entity> tree::quadtree::extract(std::unique_ptr<node>& tree, size_t object_id){
    if(not tree){return nullptr;}
    // find them remove
    auto entity = std::find_if(tree->objects_.begin(), tree->objects_.end(),
        [object_id](auto& obj) -> bool{
            if(object_id == static_cast<size_t>(obj->get_id())){
                return true;
            }
            return false;
        });
    // not in this node - keep looking
    if(entity != tree->objects_.end()){
        auto extracted = std::move(*entity);
        tree->objects_.erase(entity);
        return extracted;
    }
    // otherwise not in this node - keep looking 
    else{
        for(auto& child : tree->children_){
            auto extracted_object = extract(child, object_id);
            if(extracted_object){
                return extracted_object;
            }
        }
    }
    return nullptr;
}
void tree::quadtree::clear(std::unique_ptr<node>& tree){
    tree->objects_.clear();
    for(auto& child : tree->children_){
        clear(child);
    }
}

void tree::quadtree::notify_removals(const std::vector<entities::entity*>& removed_entities){
    for(auto entity : removed_entities){
        if(entity == nullptr){
            continue;
        }
        auto id = static_cast<size_t>(entity->get_id());
        const auto& debug_id = entity->get_debug_id();
        // A removed cafe entity must be announced so the systems holding a raw
        // pointer to it (maitre_d' tables, expediter counters/waiters) drop that
        // pointer. This is executed IN PLACE, not queued: notify_removals runs
        // while the entity is still alive, but the caller destroys it immediately
        // afterwards. A queued removal would be handled a frame later, by which
        // point the handlers would dereference (via get_id()) a freed entity.

        // TODO: fix this pattern - should not belong in the quadtree
        if(debug_id.starts_with(entity_config::table_debug_id_prefix)){
            events::removed_table removed_table{id};
            event_interface::execute_event(removed_table);
        }
        else if(debug_id.starts_with(entity_config::food_counter_debug_id_prefix)){
            events::removed_food_counter removed_food_counter{id};
            event_interface::execute_event(removed_food_counter);
        }
        else if(debug_id.starts_with(entity_config::waiter_dog_debug_id_prefix)){
            events::removed_waiter removed_waiter{id};
            event_interface::execute_event(removed_waiter);
        }
        else if(debug_id.starts_with(entity_config::customer_dog_debug_id_prefix)){
            events::removed_customer removed_customer{id};
            event_interface::execute_event(removed_customer);
        }
        else if(debug_id.starts_with(entity_config::dishwasher_debug_id_prefix)){
            events::removed_dishwasher removed_dishwasher{id};
            event_interface::execute_event(removed_dishwasher);
        }
    }
}

// ---------------- quadtree - inspection ----------------
int tree::quadtree::height(std::unique_ptr<node>& tree) {
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
size_t tree::quadtree::size(std::unique_ptr<node>& tree) {
    auto empty = is_empty(tree);
    if (empty) { return 0; }
    if(not tree){
        return 0;
    }
    else {
        auto t_size = tree->objects_.size();
        for (auto& child : tree->children_) {
            t_size += size(child);
        }
        return t_size;
    }
    return 0;
}

size_t tree::quadtree::num_nodes(std::unique_ptr<node>& tree){
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

bool tree::quadtree::is_empty(std::unique_ptr<node>& tree) {
    // check the current list 
    auto empty = tree->objects_.size() == 0 ? true : false;
    // check the children
    if (not empty) { return false; }
    for (auto& child : tree->children_) {
        if (not is_empty(child)) {
            return false;
        }
    }
    return true;
}
bool tree::quadtree::is_root(std::unique_ptr<node>& tree){
    return tree->depth_ == 0  ? true : false;
}

// ---------------- quadtree - collision and interaction ----------------
bool tree::quadtree::is_there_collision(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, int id){
    // starts with the whole if node case
    if(not tree) {
        return false;
    }
    // then check the objects
    for(auto & obj : tree->objects_){
        // it is checking collision with itself, make sure that is not the case
        if(id != obj->get_id() and bounds.check_collision(obj->get_hitbox())){
            return true;
        }
    }
    // if not colliding with something in the current node, then check the children, but only those that 
    // could contain the entity
    for(auto & child : tree->children_){
        if(node_contains_object(child->bounds_, bounds.get_box())){
            if(is_there_collision(child, bounds, id)){
                return true;
            }
        }
    }
    return false;
}

bool tree::quadtree::is_leaf(std::unique_ptr<node>& tree) {
    return tree->children_.size() == 0 ? true : false;
}
entities::entity* tree::quadtree::get_entity(std::unique_ptr<node>& tree, size_t id){
    if(not tree) {return nullptr;}
    for(auto & entity : tree->objects_){
        if(static_cast<size_t>(entity->get_id()) == id){
            return entity.get();
        }
    }
    for(auto & child : tree->children_){
        auto result = get_entity(child, id);  // Store the result
        if(result) {
            return result;  // Only return if found
        }
    }
    
    return nullptr;
}

void tree::quadtree::perform_interactions(std::unique_ptr<node>& tree, entities::entity* entity){

    // iterate until node containing object is found
    // in the correct node, checck collisions using hitboxes 

    // perform any identified collisions
    if(not tree) {
        return;
    }
    auto hitbox_rec = entity->get_hitbox().get_box();

    if(node_contains_object(tree->bounds_, hitbox_rec)){

        // check the objects
        for(auto & other_entity : tree->objects_){
            if(entity->check_collision(other_entity->get_hitbox()) and entity != other_entity.get()){
                entity->interact(*other_entity);
            }
        }
    }
    else{
    }
    for(auto & child : tree->children_){
        perform_interactions(child, entity);
    }
    // check the children

}


// i suspect some issues with this, i think it is resetting the pointer 
// but then the parent is now holding onto a null pointer

// ---------------- quadtree - maintenance ----------------
// i need to remove it from the parent's children list
void tree::quadtree::prune_leaves(std::unique_ptr<node>& tree, double delta) {
    // you're thinking about it wrong i think 
        if (is_leaf(tree) and not is_root(tree) 
            and is_empty(tree)) {
            tree->life_ += short(delta);
            if (tree->life_ >= NODE_LIFETIME) {
                tree.reset(); // but not removed from the 
                return;
            }
        }
        else {
            // if not a leaf node, or is not empty, reset the life
            tree->life_ = 0;
            for (auto& child : tree->children_) {
                prune_leaves(child, delta);
            }
            
            // cull the null children
            auto new_end = std::remove_if(tree->children_.begin(), tree->children_.end(), 
            [](auto& child) -> bool {
                return not child;
            });
            tree->children_.erase(new_end, tree->children_.end());
        }
    return;
} 

std::unique_ptr<tree::quadtree::node> tree::quadtree::copy_tree(node* tree, std::unique_ptr<node>* parent){
    (void) parent;
    if(not tree){
        return nullptr;
    }
    auto copy = std::make_unique<node>();
    
    // copy the obj_bounds, life, depth
    copy->bounds_ = tree->bounds_;
    copy->depth_ = tree->depth_;
    copy->life_ = tree->life_;
    // and then the objects, deep copy 
    for(auto & obj : tree->objects_){
        copy->objects_.push_back(std::move(obj));
    }
    auto new_parent = &copy;
    for(auto & child : tree->children_){
        copy->children_.push_back(copy_tree(child.get(), new_parent));
    }
    return copy;
}
void tree::quadtree::traverse_tree(std::unique_ptr<node>& tree){
		// print the box of the node 
		if(not tree){
			return;
		}
		for(auto& object : tree->objects_){
            (void) object;
		}
		for(auto& child : tree->children_){
			traverse_tree(child);
		}
		// print its children
		return;
}

std::vector<int> tree::quadtree::update(std::unique_ptr<node>& tree, float delta, int frame,
                                        std::vector<std::unique_ptr<entities::entity>>& graveyard){
    if(not tree) {return {};}
    std::vector<int> to_remove = {};
    for(auto it = tree->objects_.begin(); it != tree->objects_.end();){
        int update_result = (*it)->update(delta, frame);
        // dead wins - an entity can report it alongside movement.
        if(update_result & entities::status_codes::dead){
            notify_removals(std::vector<entities::entity*>{it->get()});
            to_remove.push_back((*it)->get_id());
            next_ids_.push(static_cast<size_t>((*it)->get_id()));
            graveyard.push_back(std::move(*it));
            it = tree->objects_.erase(it);
            continue;
        }
        // completed_path doesn't move the dog itself, but re-checking bounds on
        // it too is cheap insurance against a state repositioning the dog
        // during a leg transition and going stale in the wrong quadrant.
        if(update_result & (entities::status_codes::moved
                          | entities::status_codes::completed_path)){
            if(not node_contains_object(tree->bounds_, (*it)->get_hitbox().get_box())){
                auto entity = std::move(*it);
                it = tree->objects_.erase(it);
                insert(root_, std::move(entity));
                continue;
            }
        }
        ++it;
    }
    for(auto & child : tree->children_){
        auto sub_remove = update(child, delta, frame, graveyard);
        to_remove.insert(to_remove.end(), sub_remove.begin(), sub_remove.end());
    }
    return to_remove;
}
void tree::quadtree::identify_collisions(std::unique_ptr<node>& tree , std::vector<entities::entity*> parent_entities){
    if(not tree) {return;}
    // check for collisions with objects from parent nodes
    // basic debugging for tests
    for(auto& parent_entity : parent_entities){
        for(auto& entity : tree->objects_){
            // check for collisions between parent entity and entity
            auto parent_rectangle = parent_entity->get_hitbox().get_box();

            auto entity_rectangle = entity->get_hitbox().get_box();
            if(CheckCollisionRecs(parent_rectangle, entity_rectangle)){
                // interact
                parent_entity->interact(*entity);             
            }
        }
    }
    // check within the node, avoid duplicate checks and self checks 
    // so if object 1 is checked against 2 ,
    // then it avoids checking object two against 1, and so on
    if(tree->objects_.size() > 1 ){
        for(size_t i = 0; i < tree->objects_.size() - 1; ++i){
            for(size_t j = i + 1; j < tree->objects_.size(); ++j){
                auto i_rectangle = tree->objects_[i]->get_hitbox().get_box();

                auto j_rectangle = tree->objects_[j]->get_hitbox().get_box();

                if(CheckCollisionRecs(i_rectangle, j_rectangle)){
                    tree->objects_[i]->interact(*tree->objects_[j]);
                }            
            }
        }
    }
    // then append this node into parent objects and pass to children
    std::for_each(tree->objects_.begin(), tree->objects_.end(), 
    [&parent_entities] (auto& entity) -> void {
        parent_entities.push_back(entity.get());
    });

    // and recurse through the children
    std::for_each(tree->children_.begin(), tree->children_.end(), 
    [&parent_entities, this] (auto & child){
        identify_collisions(child, parent_entities);
    });
}

// ----------------------- ecs_quadtree ----------------------- //

// ---------------- ecs_quadtree - node geometry ----------------
hitbox::hitbox* tree::ecs_quadtree::bounds_for(size_t entity_id){
    auto* collision = component_managers::collision_manager_.get_component(entity_id);
    if(collision == nullptr){ return nullptr; }
    return &collision->get_hitbox_component().get_hitbox();
}
std::optional<Rectangle> tree::ecs_quadtree::interaction_bounds_for(size_t entity_id){
    auto* interactable = component_managers::interactable_manager_.get_component(entity_id);
    auto hitbox = bounds_for(entity_id);

    if(not hitbox or not interactable) {return std::nullopt; }
    return interactable->get_interaction_box(hitbox->get_box());
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
int tree::ecs_quadtree::is_there_collision(std::unique_ptr<node>& tree, Vector2 position, size_t id){
    if(not tree) {return game_config::empty_entity;}
    for(size_t entity : tree->entities_){
        if(entity == id){ continue; }
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
            int entity = is_there_collision(child, position, id);
            if(entity != game_config::empty_entity){
                return entity;
            }
        }
    }
    return game_config::empty_entity;
}
int tree::ecs_quadtree::is_there_collision(std::unique_ptr<node>& tree, Rectangle box, size_t id){
    if(not tree) {return game_config::empty_entity;}
    for(size_t entity : tree->entities_){
        if(entity == id){ continue; }
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
            int entity = is_there_collision(child, box, id);
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
        auto other_box_opt = interaction_bounds_for(entity_id);
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
