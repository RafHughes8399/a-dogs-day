/**
 *  the core data structure that underpins entity management and rendering
 *  will include some form of occlusion cullign (don't draw objects that cannot be seen / are blocked completely by others)
 */
#ifndef QUADTREE_H
#define QUADTREE_H
// std includes
#include <vector>
#include <memory>
#include <algorithm>
// raylib includes 
#include "raylib.h"
#include "raymath.h"
#include "raglib.h"
// project includes
#include "entities.h"
#include "config.h"
#define MAX_DEPTH 4
#define NODE_LIFETIME 30
#define CHILDREN 4

#define WORLD_MIN Vector2Zero()
#define WORLD_MAX Vector2 {config::world_x, config::world_y}
#define WORLD_BOX raglib::bounding_box_2{WORLD_MIN, WORLD_MAX}

namespace tree{
    class quadree {
    protected:
        // node definition
        struct node {
            std::vector<std::unique_ptr<entities::entity>> objects_;
            std::vector<std::unique_ptr<node>> children_;
            raglib::bounding_box_2 bounds_;
            int depth_;
            short life_; // how long a node has lived without any objects
            std::unique_ptr<node>* parent_;
        
            friend bool operator==(const node& a, const node& b) {
                return Vector2Equals(a.bounds_.min, b.bounds_.min) && 
                Vector2Equals(a.bounds_.max, b.bounds_.max);
            }
        };
    private:
        // members 
        std::unique_ptr<node> root_;
        int max_depth_;
        size_t next_id_;
        // methods
        // containment checks
        bool node_contains_object(raglib::bounding_box_2& node, raglib::bounding_box_2& object);
        int object_contained_by_child(raglib::bounding_box_2& node, raglib::bounding_box_2& object);
        // child node construction
        bool is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child);
        void build_child(std::unique_ptr<node>& tree, int child_to_build);
        // object insert and erase
        void insert(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& object);
        void insert(std::unique_ptr<node>& tree, std::vector<std::unique_ptr<entities::entity>>& objects);
        void erase(std::unique_ptr<node>& tree, size_t object_id);
        std::unique_ptr<entities::entity> extract(std::unique_ptr<node>& tree, size_t object_id);
        
        void clear(std::unique_ptr<node>& tree);
        // object lookup
        node* find_object_node(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& object);
        entities::entity* find_object(std::unique_ptr<node>& tree, int id);
        
        // TODO change from reference wrapper
        std::vector<std::reference_wrapper<std::unique_ptr<entities::entity>>> get_objects(std::unique_ptr<node>& tree);
        
        template<typename UnaryPred>
        std::vector<std::reference_wrapper<std::unique_ptr<entities::entity>>> get_objects(std::unique_ptr<node>& tree, UnaryPred p){
            // pass the object to the predicate
            
            auto predicate_objects = std::vector<std::reference_wrapper<std::unique_ptr<entities::entity>>>{};
            if(not tree){
                return predicate_objects;
            }
            for(auto& obj : tree->objects_){
                if(p(obj)){
                    predicate_objects.push_back(obj);
                }
            }
            for(auto& child : tree->children_){
                auto child_objects = get_objects(child, p);
                for(auto child_object : child_objects){
                    predicate_objects.push_back(child_object.get());
                }
            }
            return predicate_objects;
        }
            
        // height, size,  traversal and copying
        int height(std::unique_ptr<node>& tree);
        size_t size(std::unique_ptr<node>& tree);
        size_t num_nodes(std::unique_ptr<node>& tree);
        void traverse_tree(std::unique_ptr<node>& tree);
        std::unique_ptr<node> copy_tree(node* tree, std::unique_ptr<node>* parent);
        // tree characteristics
        bool is_root(std::unique_ptr<node>& tree);
        bool is_empty(std::unique_ptr<node>& tree);
        bool is_leaf(std::unique_ptr<node>& tree);
        
        
        void prune_leaves(std::unique_ptr<node>& tree, double delta);
        
        // update and render
        void update(std::unique_ptr<node>& tree, float delta);
        void identify_collisions(std::unique_ptr<node>& tree, std::vector<entities::entity*> parent_entities);
        void render(std::unique_ptr<node>& tree);


        void move_entity(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& entity);
        std::unique_ptr<node>* find_new_parent(std::unique_ptr<node>& tree, std::unique_ptr<entities::entity>& entity);
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
        ~quadree() = default;
        // creates an empty quadree with a root node
        quadree(raglib::bounding_box_2 root_bounds, int depth=MAX_DEPTH)
        : root_(std::make_unique<node>()), max_depth_(depth), next_id_(0) {
            root_->bounds_ = root_bounds;
            root_->life_ = 0;
            root_->depth_ = 0;
            root_->parent_ = nullptr;
            // build lazily
        }
        // creates an empty quadree, then populates it with the list of objects
        template<typename InputIt>
        quadree(raglib::bounding_box_2 root_bounds, InputIt first, InputIt last)
        : quadree(root_bounds) { // initialise the root node
            for (auto i = first; i != last; ++i) {
                insert(*i);
            }
        }
        
        quadree(raglib::bounding_box_2 root_bounds, std::vector<std::unique_ptr<entities::entity>>& objects)
        : quadree(root_bounds, objects.begin(), objects.end()) {
        }
        
        // copy and move overloads, root, depth and next id
        quadree(const quadree& other)
        :  max_depth_(other.max_depth_), next_id_(other.next_id_){
            root_ = copy_tree(other.root_.get(), nullptr);
        };

        quadree(quadree&& other);
        
        quadree& operator= (const quadree& other);
        quadree& operator=(quadree&& other);
        
        // insert and erase 
        void insert(std::vector<std::unique_ptr<entities::entity>>& objs){
            insert(root_, objs);
            
        }
        void insert(std::unique_ptr<entities::entity>& obj) {
            insert(root_, obj);
            next_id_ += 1;
        }
        void erase(size_t id){
            erase(root_, id);
        }
        std::unique_ptr<entities::entity> extract(size_t id){
            return extract(root_, id);
        }
        void clear(){
            clear(root_);
        }
        // object lookup
        node* find_object_node(std::unique_ptr<entities::entity>& obj) {
            return find_object_node(root_, obj);
        }
        
        entities::entity* find_object(int id) {
            return find_object(root_, id);
        }
        // TODO change from ref wrapper to raw pointersf
        template<typename UnaryPred>
        std::vector<std::reference_wrapper<std::unique_ptr<entities::entity>>> get_objects(UnaryPred p){
            return get_objects(root_, p);
        }
        //TODO change from ref wrapper to raw pointers
        std::vector<std::reference_wrapper<std::unique_ptr<entities::entity>>> get_objects(){
            return get_objects(root_);
        }
        
        // update and render
        void update(double delta){
            update(root_, delta);
            auto parent_objects = std::vector<entities::entity*>{};
            identify_collisions(root_, parent_objects); // start with an empty list
        }

        // render the tree within a certain bounding box, default is the whole tree
        void render(){
            render(root_);
        }
        
        template<typename UnaryPred>
        void render(UnaryPred p){
            render(root_, p);
        }
        std::unique_ptr<node>& get_root() {
            return root_;
        }
        // accessors
        std::vector<std::unique_ptr<node>>& get_children() {
            return root_->children_;
        }
        size_t get_next_id(){
            return next_id_;
        }
        
        int max_depth(){
            return max_depth_;
        }
        // height and size
        int height() {
            return height(root_);
        }
        size_t size() {
            return size(root_);
        }
        size_t num_nodes(){
            return num_nodes(root_);
        }
        
        // tree properties
        bool is_leaf() {
            return is_leaf(root_);
        }
        bool is_root(){
            return is_root(root_);
        }
        bool is_empty() {
            return is_empty(root_);
        }
        
        // checks leaves for their life, prunes if need be
        void prune_leaves(double delta) {
            prune_leaves(root_, delta);
        }
        
        // for testing purposes 
        bool object_in_node(raglib::bounding_box_2& node, raglib::bounding_box_2& obj) {
            return node_contains_object(node, obj);
        }
        
        void traverse_tree(){
            traverse_tree(root_);
        }
    };
}
#endif