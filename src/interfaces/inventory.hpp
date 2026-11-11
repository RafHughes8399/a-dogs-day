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
    class inventory_page{
    public:
    ~inventory_page() = default;
    inventory_page(page_navigator page_navigator,  item_page item_page, size_t page_num = 0)
    : page_num_(page_num), page_naviagtor_(page_navigator), item_page_(item_page){}
    
    bool store_item(size_t item_id);
    private:
        size_t page_num_;
        page_navigator page_naviagtor_;
        item_page item_page_;
    };
    ~inventory() = default;
    // ! every inventory is given an empty page
    inventory(inventory_page inventory_page)
        : current_page_(0), pages_({inventory_page}){}
    inventory(const inventory& other) = delete;
    inventory(inventory&& other) = default;
        bool store_item(size_t item_id);
        bool take_item(size_t item_id);
        std::optional<size_t> take(size_t item_id);
        bool empty() const;

        void render();
        void next_page();
        void previous_page();
            
        void add_page();
        void remove_page();
    private:
        std::vector<inventory_page> pages_;
        size_t current_page_;
    };
}
#endif
            