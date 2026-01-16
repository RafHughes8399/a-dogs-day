#include "quadtree.h"
enum positions{
    top_right = 0,
    top_left = 1,
    bottom_left = 2,
    bottom_right = 3
};
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
    bool crosses_centre = (object_min.x < centre.x && centre.x < object_max.x) 
    || (object_min.y < centre.y && centre.y < object_max.y);
    
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

// insertion
void tree::quadtree::insert(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity> object){
	auto & object_bounds = object->get_hitbox();
    // check if the node contains the object, if not then immediately return
	if(! node_contains_object(tree->bounds_, object_bounds.get_box())){ 
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
    if(! tree){
        return;
    } 
    auto new_end = std::remove_if(tree->objects_.begin(), tree->objects_.end(),
        [object_id](auto& obj) -> bool{
            if(object_id == obj->get_id()){
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
    if(!tree){return nullptr;}
    // find them remove
    auto entity = std::find_if(tree->objects_.begin(), tree->objects_.end(),
        [object_id](auto& obj) -> bool{
            if(object_id == obj->get_id()){
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
int tree::quadtree::height(std::unique_ptr<node>& tree) {
    if (!tree) {
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
    if(! tree){
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
    if (! empty) { return false; }
    for (auto& child : tree->children_) {
        if (! is_empty(child)) {
            return false;
        }
    }
    return true;
}
bool tree::quadtree::is_root(std::unique_ptr<node>& tree){
    return tree->depth_ == 0  ? true : false;
}
bool tree::quadtree::is_there_collision(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, int id){
    // starts with the whole if node case
    if(! tree) {
        return false;
    }
    // then check the objects
    for(auto & obj : tree->objects_){
        // it is checking collision with itself, make sure that is not the case
        if(id != obj->get_id() && bounds.check_collision(obj->get_hitbox())){
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
    if(! tree) {return nullptr;}
    for(auto & entity : tree->objects_){
        if(entity->get_id() == id){
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
    if(! tree) {
        return;
    }
    auto hitbox_rec = entity->get_hitbox().get_box();

    if(node_contains_object(tree->bounds_, hitbox_rec)){

        // check the objects
        for(auto & other_entity : tree->objects_){
            if(entity->check_collision(other_entity->get_hitbox()) && entity != other_entity.get()){
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

// i need to remove it from the parent's children list
void tree::quadtree::prune_leaves(std::unique_ptr<node>& tree, double delta) {
    // you're thinking about it wrong i think 
        if (is_leaf(tree) && ! is_root(tree) 
            && is_empty(tree)) {
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
    if(! tree){
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
		if(!tree){
			return;
		}
		for(auto& object : tree->objects_){
		}
		for(auto& child : tree->children_){
			traverse_tree(child);
		}
		// print its children
		return;
}

std::vector<int> tree::quadtree::update(std::unique_ptr<node>& tree, float delta){
    if(! tree) {return {};}
    std::vector<int> to_remove = {};
    for(auto it = tree->objects_.begin(); it != tree->objects_.end();){
        int update_result = (*it)->update(delta);
        switch(update_result){
            case entities::status_codes::moved:
                if(! node_contains_object(tree->bounds_, (*it)->get_hitbox().get_box())){
                        auto entity = std::move(*it);
                        it = tree->objects_.erase(it);
                        insert(root_, std::move(entity));
                        // reinsert
                }
                else{
                    ++it;
                }
                break;
            case entities::status_codes::dead:
                // remove
                to_remove.push_back((*it)->get_id());
                it = tree->objects_.erase(it);
                break;
            case entities::status_codes::nothing:
                ++it;
                break;
            default:
                ++it;
                break;
        }
    }
    // Recursively update children
    for(auto & child : tree->children_){
        auto sub_remove = update(child, delta);
        to_remove.insert(to_remove.end(), sub_remove.begin(), sub_remove.end());
    }
    return to_remove;
}
void tree::quadtree::identify_collisions(std::unique_ptr<node>& tree , std::vector<entities::entity*> parent_entities){
    if(! tree) {return;}
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
        for(auto i = 0; i < tree->objects_.size() - 1; ++i){
            for(auto j = i + 1; j < tree->objects_.size(); ++j){
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
