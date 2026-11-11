#ifndef INVENTORY_H
#define INVENTORY_H
#include "items.h"
#include <cstddef>
#include <optional>
#include <vector>
namespace inventory {
    class inventory {
    public:
        ~inventory() = default;
        inventory()
        : pages_(), current_page_(0){
        }
        inventory(const inventory& other) = delete;
        inventory(inventory&& other) = default;
        bool empty() const;

        void next_page();
        void previous_page();

        void add_page(size_t root);
        void remove_page();
    private:
        std::vector<size_t> pages_;
        size_t current_page_;
    };
}
#endif
