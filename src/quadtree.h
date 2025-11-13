/**
 *  the core data structure that underpins entity management and rendering
 *  will include some form of occlusion cullign (don't draw objects that cannot be seen / are blocked completely by others)
 */
#ifndef QUADTREE_H
#define QUADTREE_H
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

#include "config.h"
#include "entities.h"
#include "raylib.h"
#include "raymath.h"
#include "raglib.h"

#define MAX_DEPTH 4
#define NODE_LIFETIME 30
#define CHILDREN 4

#define WORLD_MIN Vector2Zero()
#define WORLD_MAX Vector2 {config::world_x, config::world_y}
#define WORLD_BOX raglib::bounding_box_2(WORLD_MIN, WORLD_MAX)


// nodes have a bounding box, entities have a rectangle
namespace tree{
    class quadtree {
    protected:
        // node definition
        struct node {
            std::vector<std::unique_ptr<entities::entity>> objects_;
            std::vector<std::unique_ptr<node>> children_;
            raglib::bounding_box_2 bounds_;
            int depth_;
            short life_; // how long a node has lived without any objects
        
            friend bool operator==(const node& a, const node& b) {
                return Vector2Equals(a.bounds_.min, b.bounds_.min) && 
                Vector2Equals(a.bounds_.max, b.bounds_.max);
            }
        };
    private:
        // members 
        events::event_handler<events::move_entity> moved_entity_handler_;
        events::event_handler<events::remove_entity> removed_entity_handler_;
        queries::query_handler<queries::is_colliding_query> is_colliding_handler_;

        int max_depth_;
        size_t next_id_;
        std::unique_ptr<node> root_;

        bool is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child);
        bool is_root(std::unique_ptr<node>& tree);
        bool is_empty(std::unique_ptr<node>& tree);
        bool is_leaf(std::unique_ptr<node>& tree);
        bool is_there_collision(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, int id);
        bool node_contains_object(raglib::bounding_box_2& node, Rectangle& object);
        
        int height(std::unique_ptr<node>& tree);
        int object_contained_by_child(raglib::bounding_box_2& node, Rectangle& object);
        
        size_t num_nodes(std::unique_ptr<node>& tree);
        size_t size(std::unique_ptr<node>& tree);
        
        std::unique_ptr<entities::entity> extract(std::unique_ptr<node>& tree, size_t object_id);
        std::unique_ptr<node> copy_tree(node* tree, std::unique_ptr<node>* parent);
        
        void build_children(std::unique_ptr<node>& tree);
        void clear(std::unique_ptr<node>& tree);
        void erase(std::unique_ptr<node>& tree, size_t object_id);
        void identify_collisions(std::unique_ptr<node>& tree, std::vector<entities::entity*> parent_entities);
        void insert(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity> object);
        void prune_leaves(std::unique_ptr<node>& tree, double delta);
        void render(std::unique_ptr<node>& tree);
        void traverse_tree(std::unique_ptr<node>& tree);
        void update(std::unique_ptr<node>& tree, float delta);

        template<typename UnaryPred>
        void render(std::unique_ptr<node>& tree, UnaryPred p){
            if(not tree){
                return;
            }
            for(auto & entity : tree->objects_){
                if(p(entity)){
                    entity->render();
                }
            }
            for(auto & child : tree->children_){
                render(child, p);
            }
        }
        public:
        // CONSTRUCTORS
        ~quadtree() {
            event_interface::unsubscribe<events::move_entity>(moved_entity_handler_);
            event_interface::unsubscribe<events::remove_entity>(removed_entity_handler_);
            query_interface::unsubscribe<queries::is_colliding_query>(is_colliding_handler_);
        }
        // creates an empty quadtree with a root node
        quadtree(raglib::bounding_box_2 root_bounds, int depth=MAX_DEPTH)
        : root_(std::make_unique<node>()), max_depth_(depth), next_id_(0),
        moved_entity_handler_([this](const events::move_entity& event) -> void {on_move_event(event);}), 
        removed_entity_handler_([this](const events::remove_entity& event) -> void {on_remove_event(event);}),
        is_colliding_handler_([this](const queries::is_colliding_query& query) -> bool {return on_is_colliding_query(query);}) {
            root_->bounds_ = root_bounds;
            root_->life_ = 0;
            root_->depth_ = 0;
            
            // sub 
            event_interface::subscribe<events::move_entity>(moved_entity_handler_);
            event_interface::subscribe<events::remove_entity>(removed_entity_handler_);
            query_interface::subscribe<queries::is_colliding_query>(is_colliding_handler_);
        }
        // creates an empty quadtree, then populates it with the list of objects
        template<typename InputIt>
        quadtree(raglib::bounding_box_2 root_bounds, InputIt first, InputIt last)
        : quadtree(root_bounds) { // initialise the root node
            for (auto i = first; i != last; ++i) {
                insert(std::move(*i));
            }
        }
        
        quadtree(raglib::bounding_box_2 root_bounds, std::vector<std::unique_ptr<entities::entity>>& objects)
        : quadtree(root_bounds, objects.begin(), objects.end()) {
        }
        
        // copy and move overloads, root, depth and next id
        quadtree(const quadtree& other)
        :  max_depth_(other.max_depth_), next_id_(other.next_id_), 
        moved_entity_handler_(other.moved_entity_handler_), 
        removed_entity_handler_(other.removed_entity_handler_),
        is_colliding_handler_(other.is_colliding_handler_){
            root_ = copy_tree(other.root_.get(), nullptr);
            event_interface::subscribe<events::move_entity>(moved_entity_handler_);
            event_interface::subscribe<events::remove_entity>(removed_entity_handler_);
            query_interface::subscribe<queries::is_colliding_query>(is_colliding_handler_);

            // and resub
        };

        quadtree(quadtree&& other);
        
        quadtree& operator= (const quadtree& other);
        quadtree& operator=(quadtree&& other);
        
        std::unique_ptr<entities::entity> extract(size_t id){
            return extract(root_, id);
        }
        // tree properties
        bool is_empty() {
            return is_empty(root_);
        }
        bool is_leaf() {
            return is_leaf(root_);
        }
        bool is_root(){
            return is_root(root_);
        }
        // for testing purposes 
        bool object_in_node(raglib::bounding_box_2& node, Rectangle& obj) {
            return node_contains_object(node, obj);
        }
        int height() {
            return height(root_);
        }
        int max_depth(){
            return max_depth_;
        }
        // height and size
        size_t get_next_id(){
            return next_id_;
        }    
        size_t num_nodes(){
            return num_nodes(root_);
        }
        size_t size() {
            return size(root_);
        }
        std::unique_ptr<node>& get_root() {
            return root_;
        }
        // accessors
        std::vector<std::unique_ptr<node>>& get_children() {
            return root_->children_;
        }
        // checks leaves for their life, prunes if need be
        // insert and erase 
        void clear(){
            clear(root_);
        }
        void erase(size_t id){
            erase(root_, id);
        }
        void insert(std::unique_ptr<entities::entity> obj) {
            insert(root_, std::move(obj));
            next_id_ += 1;
        }

        void on_move_event(const events::move_entity& event){
            size_t id = event.get_id();
            // remove and reinsert
            auto entity = extract(root_, id);
            insert(root_, std::move(entity));
        }
        void on_remove_event(const events::remove_entity& event){
            size_t id = event.get_id();
            erase(root_, id);
        }

        bool on_is_colliding_query(const queries::is_colliding_query& query){
            std::cout << "get query info" << std::endl;
            auto bounds = query.get_bounds();
            auto id = query.get_id();
            std::cout << "use bounds, check collision" << std::endl;
            return is_there_collision(root_, bounds, id);
        }
        void prune_leaves(double delta) {
            prune_leaves(root_, delta);
        }
        void traverse_tree(){
            traverse_tree(root_);
        }
        // update and render
        // render the tree within a certain bounding box, default is the whole tree
        void render(){
            render(root_);
        }
        template<typename UnaryPred>
        void render(UnaryPred p){
            render(root_, p);
        }
        void update(float delta){
            update(root_, delta);
            auto parent_objects = std::vector<entities::entity*>{};
            identify_collisions(root_, parent_objects); // start with an empty list
        }
    };
}
#endif