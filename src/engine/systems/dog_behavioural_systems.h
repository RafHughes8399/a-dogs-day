#ifndef DOG_BEHAVIOURAL_SYSTEMS_H
#define DOG_BEHAVIOURAL_SYSTEMS_H
#include <vector>
#include "config.h"
#include "raylib.h"
#include "raglib.h"
#include <optional>
namespace dbs {
    class customer_arrival_system{
        public:
                    // TODO (25 / 8 / 26) must listen to table construction and deletion, can create a new event for it and update teh builders
                    // TODO and destroyers to emit those events
            ~customer_arrival_system() = default;
            customer_arrival_system() = default;
            customer_arrival_system(const customer_arrival_system& other) = default;
            customer_arrival_system(customer_arrival_system&& other) = default;
            customer_arrival_system& operator=(const customer_arrival_system& other) = default;
            customer_arrival_system& operator=(customer_arrival_system&& other) = default;
                    
                    // create_dog
                    // destroy_dog

            void update(float delta);
            void create_customer_dog();
            void destroy_customer_dog(size_t id);
            void register_customer(size_t id);
            void unregister_customer(size_t id);
            void register_table(size_t id);
            void unregister_table(size_t id);
                
                    
            bool free_tables();
            int pick_table();
            int pick_customer();
            void customer_cleanup();
            void send_customer_to_table();
#ifdef DOG_DAYS_TESTING
                    const std::vector<size_t>& get_customers() const{
                        return customers_;
                    }
                    const std::vector<size_t>& get_tables() const{
                        return tables_;
                    }
#endif
                // teardown between test scenarios - the singleton outlives them
            void clear(){
                customers_.clear();
                tables_.clear();
                time_since_dog_ = 0.0f;
            }
                // check dog enter cafe
                //
        private:
            // const Rectangle cafe_entrace_;
            float time_since_dog_ = 0.0f;
            std::vector<size_t> customers_;
            std::vector<size_t> tables_;
    };
    class waiter_idling_system {
        public:
            ~waiter_idling_system() = default;
            waiter_idling_system() = default;
            waiter_idling_system(const waiter_idling_system& other) = default;
            waiter_idling_system(waiter_idling_system&& other) = default;
        
            waiter_idling_system& operator=(const waiter_idling_system& other) = default;
            waiter_idling_system& operator=(waiter_idling_system&& other) = default;
        
            void register_waiter(size_t waiter_id);
            void unregister_waiter(size_t waiter_id);
            void clear();


            bool is_idle(size_t waiter);
            std::optional<Rectangle> determine_idle_bounds(size_t waiter);
            size_t pick_direction();
            void build_paths(size_t waiter, size_t direction, Rectangle bounds);
            void update(float delta);
            
        private:
            std::vector<size_t> waiters_;
    };
}

#endif
