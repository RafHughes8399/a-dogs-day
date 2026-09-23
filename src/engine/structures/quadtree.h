/**
 *  the core data structure that underpins entity management and rendering
 *  will include some form of occlusion cullign (don't draw objects that cannot be seen / are blocked completely by others)
 */
#ifndef QUADTREE_H
#define QUADTREE_H
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <iostream>

#include "component.h"
#include "config.h"
#include "hitbox.h"
#include "queries.h"
#include "query_interface.h"
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
    inline const std::function<bool(size_t)> any_entity = [](size_t) -> bool { return true; };

    // TODO (25 / 8 / 26) RENAME AFTER REFACTOR IS COMPLETE - replaces quadtree once level is gone.
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
        std::optional<Rectangle> interactor_bounds_for(size_t entity_id);
        bool is_child_built(std::unique_ptr<node>& tree, std::unique_ptr<node>& child);
        bool is_root(std::unique_ptr<node>& tree);
        bool is_empty(std::unique_ptr<node>& tree);
        bool is_leaf(std::unique_ptr<node>& tree);
        bool is_there_collision(std::unique_ptr<node>& tree, hitbox::hitbox& bounds, size_t id);
        int is_there_collision(std::unique_ptr<node>& tree, Vector2 position, size_t id,
            const std::function<bool(size_t)>& filter);
        int is_there_collision(std::unique_ptr<node>& tree, Rectangle box, size_t id,
            const std::function<bool(size_t)>& filter);
        int is_there_interaction(std::unique_ptr<node>& tree, Rectangle box, size_t id);
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
        int check_collision(size_t id, Vector2 position,
            const std::function<bool(size_t)>& filter = any_entity){
            return is_there_collision(root_, position, id, filter);
        }
        int check_collision(size_t id, Rectangle box,
            const std::function<bool(size_t)>& filter = any_entity){
            return is_there_collision(root_, box,  id, filter);
        }
        int check_interaction(size_t id, Rectangle box){
            return is_there_interaction(root_, box, id);
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
