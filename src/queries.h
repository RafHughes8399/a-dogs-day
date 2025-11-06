#ifndef QUERIES_H
#define QUERIES_H

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "raglib.h"
namespace queries{
    enum ids{
        collision = 0,
        size = 1
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

    class collision_query : public query{
        public:
            ~collision_query() = default;
            collision_query(raglib::bounding_box_2 bounds)
            : query(ids::collision), bounds_(bounds){};

            static const int get_static_type(){
                return ids::collision;
            }

            raglib::bounding_box_2 get_bounds(){
                return bounds_;
            }

        private:
            raglib::bounding_box_2 bounds_;
    };

    class query_handler_interface{
        public:
            virtual ~query_handler_interface() = default;
            void execute(const query& q){
                call_query(q);
            }
            virtual const int get_type() const = 0;
        protected:
            virtual void call_query(const query& q) = 0;
    };

    template<typename Q> // Q for query
    class query_handler : public query_handler_interface{
        public:
            ~query_handler() = default;
            query_handler(std::function<bool(const Q& q)> handle)
            : handler_type_(handler_type), handler_(handle){};
	        
            query_handler(const query_handler& other) = default;
		    query_handler(query_handler&& other) = default;
		
		    query_handler& operator=(const query_handler& other) = default;
		    query_handler& operator=(query_handler&& other) = default;

            void call_query(cosnt query& q) override{
                if(q.get_type() == Q::get_static_type()){
                        handler_(static_cast<const Q&>(q));
                }
            }
            const int get_type() const override{
                return handler_type_;
            }
            bool operator==(const query_handler& other){
                return handler_type_ == other.handler_type_;
            }
        private:
            std::function<bool(const Q& q)> handler_;
            const int handler_type_;
    };

    class query_executor{
        public:
            void subscribe(int query_key, std::unique_ptr<query_handler_interface> handler);
            void unsubscribe(int query_key, const int handler);
            void execute_query(const query& query);
        private:
            std::unordered_map<int, std::vector<std::unique_ptr<query_handler_interface>>> subscriber_map_;
    };
}
#endif
