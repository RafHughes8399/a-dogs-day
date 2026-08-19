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

#include "component.h"
#include "config.h"
#include "hitbox.h"
#include "queries.h"
#include "query_interface.h"
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
                return Vector2Equals(a.bounds_.min, b.bounds_.min) and 
                Vector2Equals(a.bounds_.max, b.bounds_.max);
            }
        };
    private:
        // event and query handlers 
        events::event_handler<events::move_entity> moved_entity_handler_;
        events::event_handler<events::interact_entity> interact_entity_handler_;
        
        queries::query_handler<queries::is_colliding_query, bool> is_colliding_handler_;

        int max_depth_;
        std::queue<size_t> next_ids_;
        size_t next_id_;
        std::unique_ptr<node> root_;

        bool is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child);
        bool is_root(std::unique_ptr<node>& tree);
        bool is_empty(std::unique_ptr<node>& tree);
        bool is_leaf(std::unique_ptr<node>& tree);
        bool is_there_collision(std::unique_ptr<node>& tree, Vector2 position, int id);
        bool is_there_collision(std::unique_ptr<node>& tree, Rectangle box, int id);
        bool is_there_collision(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, int id);
        bool node_contains_object(raglib::bounding_box_2& node, const Rectangle& object);
        

        entities::entity* get_colliding_entity(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, int id);
        entities::entity* get_entity(std::unique_ptr<node>& tree, size_t id);
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
        void notify_removals(const std::vector<entities::entity*>& removed_entities);
        void perform_interactions(std::unique_ptr<node>& tree, entities::entity* entity);
        void prune_leaves(std::unique_ptr<node>& tree, double delta);
        void traverse_tree(std::unique_ptr<node>& tree);
        std::vector<int> update(std::unique_ptr<node>& tree, float delta, int frame,
                                std::vector<std::unique_ptr<entities::entity>>& graveyard);

        public:
        // CONSTRUCTORS
        ~quadtree() {
            event_interface::unsubscribe<events::interact_entity>(interact_entity_handler_);
            event_interface::unsubscribe<events::move_entity>(moved_entity_handler_);
            query_interface::unsubscribe<queries::is_colliding_query, bool>(queries::bool_executor_, is_colliding_handler_);
        }
        // creates an empty quadtree with a root node
        quadtree(raglib::bounding_box_2 root_bounds, int depth=MAX_DEPTH)
        : moved_entity_handler_([this](const events::move_entity& event) -> void {on_move_event(event);}), 
        interact_entity_handler_([this](const events::interact_entity& event) -> void {on_interact_event(event);}), 
        is_colliding_handler_([this](const queries::is_colliding_query& query) -> bool {return on_is_colliding_query(query);}),
        max_depth_(depth), next_ids_(), next_id_(0), root_(std::make_unique<node>()) {
            root_->bounds_ = root_bounds;
            root_->life_ = 0;
            root_->depth_ = 0;
            
            // sub 
            event_interface::subscribe<events::interact_entity>(interact_entity_handler_);
            event_interface::subscribe<events::move_entity>(moved_entity_handler_);
            query_interface::subscribe<queries::is_colliding_query, bool>(queries::bool_executor_, is_colliding_handler_);
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
        : moved_entity_handler_(other.moved_entity_handler_), 
        interact_entity_handler_(other.interact_entity_handler_), 
        is_colliding_handler_(other.is_colliding_handler_),
        max_depth_(other.max_depth_), next_ids_(other.next_ids_), next_id_(other.next_id_){
            root_ = copy_tree(other.root_.get(), nullptr);
            event_interface::subscribe<events::interact_entity>(interact_entity_handler_);
            event_interface::subscribe<events::move_entity>(moved_entity_handler_);
            query_interface::subscribe<queries::is_colliding_query, bool>(queries::bool_executor_, is_colliding_handler_);

            // and resub
        }

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
        bool on_is_colliding_query(const queries::is_colliding_query& query){
            auto bounds = query.get_bounds();
            auto id = query.get_id();
            return is_there_collision(root_, bounds, id);
        }

        entities::entity* on_collision_query(const queries::collision_query& query){
            auto bounds = query.get_bounds();
            auto id = query.get_id();
            return get_colliding_entity(root_, bounds, id);
        }
        // height and size
        size_t get_next_id(){
            if(next_ids_.empty()){
                return next_id_;
            }
            else{
                size_t id = next_ids_.front();
                next_ids_.pop();
                return id;
                // get the front of the queu 
            }
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
            auto entity = get_entity(root_, id);
            if(entity != nullptr){
                notify_removals(std::vector<entities::entity*>{entity});
            }
            erase(root_, id);
            // add the id to the queue
            next_ids_.push(id);
        }
        void insert(std::unique_ptr<entities::entity> obj) {
            insert(root_, std::move(obj));
            if(next_ids_.empty()){
                next_id_ += 1;
            }
        }

        void on_interact_event(const events::interact_entity& event){
            size_t id = event.get_id();
            auto entity = get_entity(root_, id);
            perform_interactions(root_, entity);
            return;
        }

        void on_move_event(const events::move_entity& event){
            size_t id = event.get_id();
            // remove and reinsert
            auto entity = extract(root_, id);
            insert(root_, std::move(entity));
        }
        void prune_leaves(double delta) {
            prune_leaves(root_, delta);
        }
        void traverse_tree(){
            traverse_tree(root_);
        }

        std::vector<int> update(float delta, int frame,
                                std::vector<std::unique_ptr<entities::entity>>& graveyard){
            auto to_remove = update(root_, delta, frame, graveyard);
            auto parent_objects = std::vector<entities::entity*>{};
            identify_collisions(root_, parent_objects);
            return to_remove;
        }
    };

    // TODO: RENAME AFTER REFACTOR IS COMPLETE - replaces quadtree once level is gone.
    // same structure, holding ids. no update loop and no interact dispatch -
    // the systems own those now - and no id allocation, that is
    // entity_lifespan_system's.
    // NOTE: on_is_colliding_query is a plain method until the spatial system
    // wires it.
    class ecs_quadtree {
    protected:
        struct node {
            std::vector<size_t> entities_;
            std::vector<std::unique_ptr<node>> children_;
            raglib::bounding_box_2 bounds_;
            int depth_;
            short life_;

            friend bool operator==(const node& a, const node& b) {
                return Vector2Equals(a.bounds_.min, b.bounds_.min) and
                Vector2Equals(a.bounds_.max, b.bounds_.max);
            }
        };
    private:
        int max_depth_;
        std::unique_ptr<node> root_;

        // an entity with no collision component has no bounds and is not indexed
        hitbox::hitbox* bounds_for(size_t entity_id);

        bool is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child);
        bool is_root(std::unique_ptr<node>& tree);
        bool is_empty(std::unique_ptr<node>& tree);
        bool is_leaf(std::unique_ptr<node>& tree);
        bool is_there_collision(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, size_t id);
        int is_there_collision(std::unique_ptr<node>& tree, Vector2 position, size_t id);
        int is_there_collision(std::unique_ptr<node>& tree, Rectangle box, size_t id);
        bool node_contains_object(raglib::bounding_box_2& node, const Rectangle& object);
        bool node_contains_position(raglib::bounding_box_2& ndoe, const Vector2& position);
        bool node_overlaps_object(raglib::bounding_box_2& node, const Rectangle& object);
        bool get_colliding_entity(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, size_t id, size_t& found);
        bool contains(std::unique_ptr<node>& tree, size_t entity_id);
        bool node_bounds_of(std::unique_ptr<node>& tree, size_t entity_id, raglib::bounding_box_2& bounds);
        int depth_of(std::unique_ptr<node>& tree, size_t entity_id);
        int height(std::unique_ptr<node>& tree);
        int object_contained_by_child(raglib::bounding_box_2& node, Rectangle& object);

        size_t num_nodes(std::unique_ptr<node>& tree);
        size_t size(std::unique_ptr<node>& tree);

        std::unique_ptr<node> copy_tree(node* tree, std::unique_ptr<node>* parent);

        void build_children(std::unique_ptr<node>& tree);
        void clear(std::unique_ptr<node>& tree);
        void erase(std::unique_ptr<node>& tree, size_t entity_id);
        void insert(std::unique_ptr<node>& tree, size_t entity_id, hitbox::hitbox& bounds);
        void prune_leaves(std::unique_ptr<node>& tree, double delta);
        void traverse_tree(std::unique_ptr<node>& tree);

        public:
        ~ecs_quadtree() = default;
        ecs_quadtree(raglib::bounding_box_2 root_bounds, int depth=MAX_DEPTH)
        : max_depth_(depth), root_(std::make_unique<node>()) {
            root_->bounds_ = root_bounds;
            root_->life_ = 0;
            root_->depth_ = 0;
        }
        // non-copyable, non-movable - one system owns the tree
        ecs_quadtree(const ecs_quadtree& other) = delete;
        ecs_quadtree(ecs_quadtree&& other) = delete;

        ecs_quadtree& operator=(const ecs_quadtree& other) = delete;
        ecs_quadtree& operator=(ecs_quadtree&& other) = delete;

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
        bool object_in_node(raglib::bounding_box_2& node, Rectangle& obj) {
            return node_contains_object(node, obj);
        }
        bool contains(size_t entity_id){
            return contains(root_, entity_id);
        }
        // which node holds it - depth alone says up/down, bounds distinguish
        // siblings at the same depth. -1 / false when it is not in the tree
        int depth_of(size_t entity_id){
            return depth_of(root_, entity_id);
        }
        bool node_bounds_of(size_t entity_id, raglib::bounding_box_2& bounds){
            return node_bounds_of(root_, entity_id, bounds);
        }
        int height() {
            return height(root_);
        }
        int max_depth(){
            return max_depth_;
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
        std::vector<std::unique_ptr<node>>& get_children() {
            return root_->children_;
        }

        // insert and erase
        void clear(){
            clear(root_);
        }
        void erase(size_t entity_id){
            erase(root_, entity_id);
        }
        // the caller supplies the bounds - the hitbox is assumed to already sit
        // at the entity's position
        void insert(size_t entity_id, hitbox::hitbox& bounds) {
            insert(root_, entity_id, bounds);
        }
        void move(size_t entity_id, hitbox::hitbox& bounds){
            erase(root_, entity_id);
            insert(root_, entity_id, bounds);
        }
        int check_collision(size_t id, Vector2 position){
            return is_there_collision(root_, position, id);
        }
        int check_collision(size_t id, Rectangle box){
            return is_there_collision(root_, box,  id);
        }
        bool on_is_colliding_query(const queries::is_colliding_query& query){
            auto bounds = query.get_bounds();
            auto id = query.get_id();
            return is_there_collision(root_, bounds, static_cast<size_t>(id));
        }
        bool on_collision_query(const queries::collision_query& query, size_t& found){
            auto bounds = query.get_bounds();
            auto id = query.get_id();
            return get_colliding_entity(root_, bounds, static_cast<size_t>(id), found);
        }
        void prune_leaves(double delta) {
            prune_leaves(root_, delta);
        }
        void traverse_tree(){
            traverse_tree(root_);
        }
    };
}
#endif
