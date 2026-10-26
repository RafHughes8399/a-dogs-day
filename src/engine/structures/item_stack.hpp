#ifndef ITEM_STACK_H
#define ITEM_STACK_H
#include <cstdint>
#include <algorithm>
#include <vector>
// multi purpose item stack, usable for food counters and displaying inventory
// operates like a stack, under the hood uses an array, with a stack limit
#include "items.h"
#define STACK_LIMIT 64 // because [unint8] 
namespace item_stack{
    class item_stack{
        public:
            ~item_stack() = default;
            item_stack()
            :items_({}) {}
            
            item_stack(std::vector<items::item>& items)
            :items_(items){}

            item_stack(std::vector<size_t> item_ids)
            : item_stack(){
                for(size_t& id : item_ids){
                    items_.push_back(items::item(id));
                }
            }
            item_stack(const item_stack& other) = default;
            item_stack(item_stack&& other) = default;

            item_stack& operator=(const item_stack& other) = default;
            item_stack& operator=(item_stack&& other) = default;

            // size is the number of item objects - does not consider count 
            size_t size() const;
            // total is the number of total items - does consider count
            size_t total() const;
            bool empty() const;
            items::item& head();
            void pop();
            void push(size_t item_id);

            // * is a stack so these two only interact with the head of the list
            size_t take();
            void place(size_t item_id);
        private:
            std::vector<items::item> items_;
    };
    
}
#endif