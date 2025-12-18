#ifndef QUERIES_H
#define QUERIES_H

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "hitbox.h"
#include "raglib.h"
namespace queries{
    enum ids{
        is_colliding = 0,
        collision = 1,
        size = 2
    };
    
    class query{
        protected:
            const int type_;
        public:
            virtual ~query() = default;
            query(int id)
            : type_(id){};

            query(query&& other) = default;
            query& operator=(query&& other) = default;

            const int get_type() const{
                return type_;
            }
    };

    class is_colliding_query : public query{
        public:
            ~is_colliding_query() = default;
            is_colliding_query(hitbox::hitbox box, int id)
            : query(ids::is_colliding), box_(box), id_(id){};

            static const int get_static_type(){
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
            ~collision_query() = default;
            collision_query(hitbox::hitbox box, int id)
            : query(ids::collision), box_(box), id_(id){};

            static const int get_static_type(){
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

    class query_handler_interface{
        public:
            virtual ~query_handler_interface() = default;
            bool execute(const query& q){
                return call_query(q);
            }
        protected:
            virtual bool call_query(const query& q) = 0;
    };

    template<typename Q> // Q for query, T for type 
    class query_handler : public query_handler_interface{
        public:
            ~query_handler() = default;
            query_handler(std::function<bool(const Q& q)> handle)
            : handler_(handle){};
	        
            query_handler(const query_handler& other) = default;
		    query_handler(query_handler&& other) = default;
		
		    query_handler& operator=(const query_handler& other) = default;
		    query_handler& operator=(query_handler&& other) = default;

            bool call_query(const query& q) override{
                return handler_(static_cast<const Q&>(q));
            }
        private:
            std::function<T(const Q& q)> handler_;
    };

    class query_executor{
        public:
            void subscribe(int query_key, std::unique_ptr<query_handler_interface> handler);
            void unsubscribe(int query_key);
            bool execute_query(const query& query);
        private:
            std::unordered_map<int, std::unique_ptr<query_handler_interface>> subscriber_map_;
    };
    extern query_executor global_executor_; // bool executor, entities::entity* executor
}
#endif
