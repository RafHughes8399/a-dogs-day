#ifndef SHOP_HPP
#define SHOP_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "inventory.hpp"

namespace shop {
    class item_definition {
    public:
        ~item_definition() = default;
        item_definition(size_t id, std::string name, int price, int level_requirement)
        : id_(id), name_(std::move(name)), price_(price), level_requirement_(level_requirement){}
        item_definition(const item_definition& other) = default;
        item_definition(item_definition&& other) = default;

        item_definition& operator=(const item_definition& other) = default;
        item_definition& operator=(item_definition&& other) = default;

        size_t get_id() const;
        const std::string& get_name() const;
        int get_price() const;
        int get_level_requirement() const;
        bool is_unlocked(int level) const;
    private:
        size_t id_;
        std::string name_;
        int price_;
        int level_requirement_;
    };

    class shop {
    public:
        ~shop() = default;
        shop() = default;
        shop(const shop& other) = default;
        shop(shop&& other) = default;

        shop& operator=(const shop& other) = default;
        shop& operator=(shop&& other) = default;

        const std::vector<item_definition>& get_catalogue() const;
        bool can_buy(size_t item_id, int level, int funds) const;
        bool buy(size_t item_id, int level, int& funds, inventory::inventory& into);
    private:
        std::vector<item_definition> catalogue_;
    };
}
#endif
