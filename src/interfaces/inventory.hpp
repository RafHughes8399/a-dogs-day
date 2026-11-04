#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "items.h"
#include <cstddef>
#include <map>
#include <unordered_map>
#include <vector>
namespace inventory {
    class inventory {
    public:
        ~inventory() = default;
        inventory() = default;
        inventory(const inventory& other) = default;
        inventory(inventory&& other) = default;

        inventory& operator=(const inventory& other) = default;
        inventory& operator=(inventory&& other) = default;

        void add(size_t item_id, size_t count = 1);
        bool remove(size_t item_id, size_t count = 1);
        std::optional<size_t> take(size_t item_id);
        
        size_t count(size_t item_id) const;
        bool empty() const;
        const std::map<size_t, std::vector<items::item>>& get_items();
    private:
        std::map<size_t, std::vector<items::item>> items_;
    };
    class inventory_interface{
        private:
            class inventory_page{
                public:
                private:
                    size_t page_num_;
                    // page navigation hud element
                    // stored items hud element 
                    // like a recipe button an item button needs a count compoennt attached to it too
            };
        public:
            // store item 
            // take item
            // render 
            // update
            // get items on page
            // add page / remove page 
            // next page / prev page. could be an iterator. this is a case where it makes sens
            void next_page();
            void previous_page();

            void add_page();
            void remove_page();
        private:
            // a list of inventory pages essentially
            inventory inventory_;
            std::vector<inventory_page> pages_;
            size_t current_page_;
    };
}
#endif
