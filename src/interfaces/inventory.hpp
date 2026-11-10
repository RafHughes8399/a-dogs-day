#ifndef INVENTORY_H
#define INVENTORY_H
#include "items.h"
#include <cstddef>
#include <vector>
#include "page_navigator.hpp"
#include "item_page.hpp"
namespace inventory {
    class inventory {
    public:
        struct item_node{
            items::inventory_item item;
            item_node* prev;
            item_node* next;
            item_node(items::inventory_item item, item_node* prev = nullptr, item_node* next = nullptr)
            :item(item), prev(prev), next(next){}
        };
    class inventory_page{
    public:
    ~inventory_page() = default;
    inventory_page(size_t page_num);
    private:
        size_t page_num_;
        page_navigator page_naviagtor_;
        item_page items_;
    };
    ~inventory(){
        auto* current = head_;
        while(current != nullptr){
            auto* next = current->next;
            delete current;
            current = next;
        }
    }
    inventory()
        :head_(nullptr), current_page_(0), pages_({}){}
    inventory(items::inventory_item head_item)
        :head_(new item_node(head_item)), current_page_(0), pages_({}){}
    inventory(const inventory& other) = delete;
    inventory(inventory&& other) noexcept
        :head_(other.head_), current_page_(other.current_page_), pages_(other.pages_){
            other.head_ = nullptr;
    }
        inventory& operator=(const inventory& other) = delete;
        inventory& operator=(inventory&& other) noexcept{
            if(this != &other){
                auto* current = head_;
                while(current != nullptr){
                    auto* next = current->next;
                    delete current;
                    current = next;
                }
                head_ = other.head_;
                other.head_ = nullptr;
            }
            return *this;
        }
        bool store_item(size_t item_id);
        bool take_item(size_t item_id);
        std::optional<size_t> take(size_t item_id);
        bool empty() const;
        item_node* get_items();

        void render();
        void next_page();
        void previous_page();
            
        void add_page();
        void remove_page();
    private:
        item_node* find(size_t item_id);
        item_node* head_;
        std::vector<inventory_page> pages_;
        size_t current_page_;
    };
}
#endif
            