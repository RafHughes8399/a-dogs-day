#ifndef ITEM_STACK_H
#define ITEM_STACK_H
#include <cstdint>
#include <algorithm>
#include <vector>
// multi purpose item stack, usable for food counters and displaying inventory
// operates like a stack, under the hood uses an array, with a stack limit
#define STACK_LIMIT 64 // because [unint8] 
namespace item_stack{
    class item{
        public:
            ~item();
            item(size_t item_id, std::uint8_t count = 1)
            :item_id_(item_id), count_(count){};

            item(const item& other) = default;
            item(item&& other) = default;

            item& operator=(const item& other) = default;
            item& operator=(item&& other) = default;
            item& operator++(){
                ++count_;
                return *this;
            }
            item& operator++(int){
                auto temp = *this;
                ++count_;
                return temp;
            }
            item& operator--(){
                --count_;
                return *this;
            }
            item& operator--(int){
                auto temp = *this;
                --count_;
                return temp;
            }
            friend bool operator==(const item& a, const item& b){
                // TODO 26 / 08 actually implment
                return true;
            }
            size_t get_id();
            std::uint8_t get_count();
        private:
            size_t item_id_;
            std::uint8_t count_;
    };
    class item_stack{
        public:
            ~item_stack() = default;
            item_stack()
            :items_({}) {}
            
            item_stack(std::vector<item>& items)
            :items_(items){}

            item_stack(std::vector<size_t> item_ids)
            : item_stack(){
                for(size_t& id : item_ids){
                    items_.push_back(item(id));
                }
            }
            item_stack(const item_stack& other);
            item_stack(item_stack&& other);

            item_stack& operator=(const item_stack& other) = default;
            item_stack& operator=(item_stack&& other) = default;

            // size is the number of item objects - does not consider count 
            size_t size() const;
            // total is the number of total items - does consider count
            size_t total() const;
            bool empty() const;
            item& head();
            void pop();
            void push(size_t item_id);

            // * is a stack so these two only interact with the head of the list
            size_t take();
            void place(size_t item_id);
        private:
            std::vector<item> items_;
    };
    
}
#endif