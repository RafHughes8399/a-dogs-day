#ifndef QUERIES_H
#define QUERIES_H

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "hitbox.h"
#include "raglib.h"
namespace queries{
    // TODO: [queries::ids] [new query kinds for waiter self-handling] change
    // from [4 existing ids, all answered by bool/int/path_executor_] to
    // [add next_serving_target = 4, next_clearing_target = 5, size = 6 -
    // two distinct query types, matching the existing convention of one type
    // per need (is_colliding vs collision vs place_decoration vs path) rather
    // than one generic type with an internal discriminator].
    enum ids{
        is_colliding = 0,
        collision = 1,
        place_decoration = 2,
        path = 3,
        size = 4
    };

    class query{
        protected:
            const int type_;
        public:
            virtual ~query() = default;
            query(int id)
            : type_(id){}

            query(query&& other) = default;
            query& operator=(query&& other) = delete;

            int get_type() const{
                return type_;
            }
    };

    class is_colliding_query : public query{
        public:
            is_colliding_query(hitbox::hitbox box, int id)
            : query(ids::is_colliding), box_(box), id_(id){}

            static int get_static_type(){
                return ids::is_colliding;
            }

            int get_id() const{
                return id_;
            }
            hitbox::hitbox get_bounds() const{
                return box_;
            }
        private:
            hitbox::hitbox box_;
            int id_;
    };
    class collision_query : public query{
        public:
            collision_query(hitbox::hitbox box, int id)
            : query(ids::collision), box_(box), id_(id){}

            static int get_static_type(){
                return ids::collision;
            }

            int get_id() const{
                return id_;
            }
            hitbox::hitbox get_bounds() const{
                return box_;
            }
        private:
            hitbox::hitbox box_;
            int id_;
    };
    class can_place_decoration : public query{
        public:
            can_place_decoration(Rectangle rectangle, int id)
            : query(ids::place_decoration), decoration_rectanlge_(rectangle), decoration_id_(id){}

            static int get_static_type(){
                return ids::place_decoration;
            }
            Rectangle get_decoration_rectangle() const{
                return decoration_rectanlge_;
            }
            int get_decoration_id() const {
                return decoration_id_;
            }
        private:
            Rectangle decoration_rectanlge_;
            int decoration_id_;
    };
    class path_query : public query{
        public:
            path_query(Vector2 source, Vector2 destination, Vector2 direction)
            : query(ids::path), source_(source), destination_(destination), direction_(direction){}

            static int get_static_type(){
                return ids::path;
            }
            Vector2 get_source() const{
                return source_;
            }
            Vector2 get_destination() const{
                return destination_;
            }
            Vector2 get_direction() const{
                return direction_;
            }
        private:
            Vector2 source_;
            Vector2 destination_;
            Vector2 direction_;
    };
    // TODO: [queries.h, after path_query] [new query classes for the waiter
    // self-handling refactor] change from [no station-target query exists;
    // expediter picks the next station reactively inside
    // on_dog_completed_path_event] to [add:
    //   struct leg_target{ bool has_next; int station_id; Vector2 station_position; };
    //   class next_serving_target_query : public query{ ... size_t waiter_id_; };
    //   class next_clearing_target_query : public query{ ... size_t waiter_id_; };
    // Each carries only the waiter_id - the query TYPE itself already tells
    // expediter's handler which leg is asking (serving vs clearing), so the
    // handler doesn't need to guess from incidental state like
    // is_carrying_food()/dishwasher_id==empty_id the way
    // on_dog_completed_path_event does today. has_next=false on the response
    // means "job complete, go idle" - mirrors how walking_to_table's path
    // carries its own destination rather than the state re-deriving it.]

    template <typename T>
    class query_handler_interface{
        public:
            virtual ~query_handler_interface() = default;
            query_handler_interface() = default;
            query_handler_interface(const query_handler_interface& other) = default;
            query_handler_interface(query_handler_interface&& other) = default;

            query_handler_interface& operator=(const query_handler_interface& other) = delete;
            query_handler_interface& operator=(query_handler_interface&& other) = delete;
            T execute(const query& q){
                return call_query(q);
            }
        protected:
            virtual T call_query(const query& q) = 0;
    };

    template<typename Q, typename T> // Q for query, T for type 
    class query_handler : public query_handler_interface<T>{
        public:
            ~query_handler() override = default;
            query_handler(std::function<T(const Q& q)> handle)
            : handler_(handle){}
            query_handler(const query_handler& other) = default;
		    query_handler(query_handler&& other) = default;
		
		    query_handler& operator=(const query_handler& other) = default;
		    query_handler& operator=(query_handler&& other) = default;

            T call_query(const query& q) override{
                return handler_(static_cast<const Q&>(q));
            }
        private:
            std::function<T (const Q& q)> handler_;
    };
    template <typename T> // T for type 
    class query_executor{
        public:
            // ? i need to template these too ?, yes
            void subscribe(int query_key, std::unique_ptr<query_handler_interface<T>> handler){
                // find the id key
                subscriber_map_[query_key] = (std::move(handler));
                return;
            }
            void unsubscribe(int query_key){
                // find the query key
                // erase the handler
                subscriber_map_.erase(query_key);
                return;
            }
            // this cannot be bool,. this has to be something else 
            T execute_query(const query& query){
                return subscriber_map_[query.get_type()]->execute(query);
            }
        private:
            std::unordered_map<int, std::unique_ptr<query_handler_interface<T>>> subscriber_map_;
    };
    extern query_executor<bool> bool_executor_; // bool executor, entities::entity* executor
    extern query_executor<int> int_executor_;
    extern query_executor<std::vector<Vector2>> path_executor_;
    // TODO: [queries.h, global executors] [waiter self-handling refactor]
    // change from [three executors, one per return type already in use] to
    // [add extern query_executor<leg_target> leg_target_executor_; - same
    // shape as the three above, just a fourth T. Defined/instantiated in
    // queries.cpp alongside the others. expediter registers one handler per
    // query type (next_serving_target/next_clearing_target) on this single
    // executor, same as level_graph registering both is_colliding and path
    // handlers on their respective executors today.]
}
#endif
