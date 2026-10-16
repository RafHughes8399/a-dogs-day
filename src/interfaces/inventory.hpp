#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <cstddef>
#include <unordered_map>

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
        size_t count(size_t item_id) const;
        bool empty() const;
        const std::unordered_map<size_t, size_t>& contents() const;
    private:
        std::unordered_map<size_t, size_t> items_;
    };
}
#endif
