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
    enum ids{
        is_colliding = 0,
        collision,
        place_decoration,
        path,
        size
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
}
#endif
