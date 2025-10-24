#include "quadtree.h"
enum positions{
    top_right = 0,
    top_left = 1,
    bottom_left = 2,
    bottom_right = 3
};
// containment checks
bool tree::quadree::node_contains_object(raglib::bounding_box_2& node, raglib::bounding_box_2& object){
    // compare the bounding box of the node and the object
    return (object.min.x >= node.min.x && object.min.y >= node.min.y)
    && (object.max.x <= node.max.x && object.max.y <= node.max.y);
}
// return the child "index" that the object can fit into, if -1 then no child can fit the object
int tree::quadree::object_contained_by_child(raglib::bounding_box_2& node, raglib::bounding_box_2& object){
    // check if the object will fit into potential children of the node 
    auto centre = Vector2Add(node.max, node.min);
    centre = Vector2Scale(centre, 0.5f);

    // first check if the object crosses the centre of any axis, if it does then no child will fit it
    bool crosses_centre = (object.min.x < centre.x && centre.x < object.max.x) 
    || (object.min.y < centre.y && centre.y < object.max.y);
    
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
bool tree::quadree::is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child){
    for(auto& c : tree->children_){
        if(*c == *child){return true;}
    }
    return false;
}

void tree::quadree::build_child(std::unique_ptr<node>& tree, int child_to_build){
    auto centre = Vector2Add(tree->bounds_.max, tree->bounds_.min);
    centre = Vector2Scale(centre, 0.5f);

    // create the child
    auto child = std::make_unique<node>();
    child->depth_ = tree->depth_ + 1;
    child->life_ = 0;
    child->parent_ = &tree;

    // based on the "index" order used ni the method above to select the appropraite bounding box for the child
    switch (child_to_build)
    {
    case positions::top_right:
        child->bounds_ =  raglib::bounding_box_2{centre, tree->bounds_.max};
        break;
    case positions::top_left:
        child->bounds_ = raglib::bounding_box_2{Vector2{tree->bounds_.min.x, centre.y},  Vector2{centre.x, tree->bounds_.max.y}};
        break;
    case positions::bottom_left:
        child->bounds_ = raglib::bounding_box_2{tree->bounds_.min, centre};
        break;
    case positions::bottom_right:
       child->bounds_ = raglib::bounding_box_2{Vector2{centre.x, tree->bounds_.min.y},  Vector2{tree->bounds_.max.x, centre.y}};
        break;
    default:
        break;
    }

    // if the child has not been built, build it
    if(! is_child_built(tree, child)){
        tree->children_.push_back(std::move(child));
    }
}

// insertion
void tree::quadree::insert(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& object){
	auto object_bounds = object->get_bounds();
    // check if the node contains the object, if not then immediately return
	if(! node_contains_object(tree->bounds_, object_bounds)){ return; }
	
    else{
        // if at the max depth then insert, no further children can be constructed
        if(tree->depth_ == max_depth_){
			tree->objects_.push_back(std::move(object));
			return;
		}

        // if not all children for the tree have been built
		else if(tree->children_.size() < CHILDREN){
			// check which child needs to be built, and build that child
            auto child_to_build = object_contained_by_child(tree->bounds_, object_bounds);
            if(child_to_build != -1){
                    build_child(tree, child_to_build);
            }
			else{
				// if no child need be built, then insert into the node
				tree->objects_.push_back(std::move(object));
                return;
			}
		}
        // recursively iterate through the children
	    for (auto& child : tree->children_) {
		// if does fit in a child, recursively insert
            if (node_contains_object(child->bounds_, object_bounds)) {
                insert(child, object);
                return;
            }
		}
        // if this point is reached, there are no children that the object can fit into so insert into the node
        tree->objects_.push_back(std::move(object));
	}
}

void tree::quadree::insert(std::unique_ptr<node>& tree, std::vector<std::unique_ptr<entities::entity>>& objects){
    // quite similar logic to before, just goes with a list of objects instead, level by level, less overall
    // traversal cost than inserting one by one
    //TODO: implement
    (void) tree;
    (void) objects;
    return;

}

void tree::quadree::erase(std::unique_ptr<node>& tree, size_t object_id){
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

std::unique_ptr<entities::entity> tree::quadree::extract(std::unique_ptr<node>& tree, size_t object_id){
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
            extract(child, object_id);
        }
    }
}
void tree::quadree::clear(std::unique_ptr<node>& tree){
    tree->objects_.clear();
    for(auto& child : tree->children_){
        clear(child);
    }
}

// object lookup

tree::quadree::node* tree::quadree::find_object_node(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& object) {
    if (!tree) {
        return nullptr;
    }
    // iterate through the objects in the node, if equal, return a pointer to the node
    for (auto& obj : tree->objects_) {
        if (*obj == *object) {
            return tree.get();
        }
    }
    // recurse through the children of the node
    for (auto& child : tree->children_) {
        auto result =  find_object_node(child, object);
        if (result != nullptr) {
            return result;
        }
    }
    return nullptr;
}

entities::entity* tree::quadree::find_object(std::unique_ptr<node>& tree, int id) {
    if (!tree) return nullptr;

    // Check if object is in current node
    for(auto& object : tree->objects_){
        if(object->get_id() == id){
            return object.get();
        }
    }
    // Recursively search children
    for (auto& child : tree->children_) {
        auto result = find_object(child, id);
        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;  // Not found
}
// TODO change from reference wrapper
std::vector<std::reference_wrapper<std::unique_ptr<entities::entity>>> tree::quadree::get_objects(std::unique_ptr<node>& tree){
    return get_objects(tree, [](auto& object) -> bool{
        (void) object;
        return true;
    });
}
int tree::quadree::height(std::unique_ptr<node>& tree) {
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
size_t tree::quadree::size(std::unique_ptr<node>& tree) {
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

size_t tree::quadree::num_nodes(std::unique_ptr<node>& tree){
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

bool tree::quadree::is_empty(std::unique_ptr<node>& tree) {
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
bool tree::quadree::is_root(std::unique_ptr<node>& tree){
    return tree->depth_ == 0  ? true : false;
}
bool tree::quadree::is_leaf(std::unique_ptr<node>& tree) {

    return tree->children_.size() == 0 ? true : false;
}


// i suspect some issues with this, i think it is resetting the pointer 
// but then the parent is now holding onto a null pointer

// i need to remove it from the parent's children list
void tree::quadree::prune_leaves(std::unique_ptr<node>& tree, double delta) {
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

std::unique_ptr<tree::quadree::node> tree::quadree::copy_tree(node* tree, std::unique_ptr<node>* parent){
    if(! tree){
        return nullptr;
    }
    auto copy = std::make_unique<node>();
    
    // copy the bounds, life, depth
    copy->bounds_ = tree->bounds_;
    copy->depth_ = tree->depth_;
    copy->life_ = tree->life_;
    copy->parent_ = parent;
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
void tree::quadree::traverse_tree(std::unique_ptr<node>& tree){
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



void tree::quadree::update(std::unique_ptr<node>& tree, float delta){
    if(! tree) {return;}
    for(auto& object : tree->objects_){
        auto update = object->update(delta);

        switch (update){
            case entities::status_codes::moved:
                // if moved then append to moved objects
                // check if moved out of the current node
                move_entity(tree, object); // moves the entity 
                // my thoughts is here to do somthing along the lines of 
                //check_collisions(object)
                break;
            case entities::status_codes::dead:
                erase(tree, object->get_id());
            default:
                break;
        }
    }
    for(auto & child : tree->children_){
        update(child, delta);
    }
    return;
}
void tree::quadree::identify_collisions(std::unique_ptr<node>& tree , std::vector<entities::entity*> parent_entities){
    if(! tree) {return;}
    // check for collisions with objects from parent nodes
    // basic debugging for tests
    for(auto& parent_entity : parent_entities){
        for(auto& entity : tree->objects_){
            // check for collisions between parent entity and entity
            auto parent_bounds = parent_entity->get_bounds();
            auto parent_rectangle = Rectangle{parent_entity->get_position().x, parent_entity->get_position().y, 
            parent_bounds.max.x - parent_bounds.min.x, parent_bounds.max.y - parent_bounds.min.y};

            auto entity_bounds = entity->get_bounds();
            auto entity_rectangle = Rectangle{entity->get_position().x, entity->get_position().y, 
            entity_bounds.max.x - entity_bounds.min.x, entity_bounds.max.y - entity_bounds.min.y};
            
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
                auto i_bounds = tree->objects_[i]->get_bounds();
                auto i_rectangle = Rectangle{tree->objects_[i]->get_position().x, tree->objects_[i]->get_position().y, 
                i_bounds.max.x - i_bounds.min.x, i_bounds.max.y - i_bounds.min.y};

                auto j_bounds = tree->objects_[j]->get_bounds();
                auto j_rectangle = Rectangle{tree->objects_[j]->get_position().x, tree->objects_[j]->get_position().y, 
                j_bounds.max.x - j_bounds.min.x, j_bounds.max.y - j_bounds.min.y};

                if(CheckCollisionRecs(i_rectangle, j_rectangle)){
                    // interact
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
void tree::quadree::render(std::unique_ptr<node>& tree){
    // if null tree skip 
    if(! tree){
        return; 
    }
    // if tree in bounds, render objects 
    for(auto & object : tree->objects_){
            object->render();
    }
    // then iterate through children, only render those in bounds
    for(auto & child : tree->children_){
        render(child);
    }
}

void tree::quadree::move_entity(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& entity){
    // check the bounding box

    // if not still
    if(! node_contains_object(tree->bounds_, entity->get_bounds())){
        // extract 
        auto extracted_entity = extract(tree, entity->get_id());
        // find the appropraite parent
        auto new_parent = find_new_parent(tree, extracted_entity);
        // and reinsert at the parent
        insert(*new_parent, extracted_entity);
    }
    else{
        // do nothing, it does not need to move node 
    }
    return;
}

std::unique_ptr<tree::quadree::node>* tree::quadree::find_new_parent(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& entity){
    auto current = &tree;
    auto entity_bounds = entity->get_bounds();
    while(! node_contains_object((*current)->bounds_, entity_bounds)){
        current = (*current)->parent_;
    }
    return current;
}