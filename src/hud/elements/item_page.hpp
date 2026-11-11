#ifndef ITEM_PAGE
#define ITEM_PAGE
#include "raylib.h"
#include "raymath.h"
#include "items.h"

//!  placeholders - will change when testing
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 800
#define DEFAULT_X  100
#define DEFAULT_Y 100
class item_page{
    public:
        ~item_page() = default;
        item_page(float scale = 1.0f)
        : page_dimensions_(Rectangle{DEFAULT_X, DEFAULT_Y, DEFAULT_WIDTH, DEFAULT_HEIGHT}), last_insert_row_(0), last_insert_column_(0), items_({}){}
        item_page(const item_page& other) = default;
        item_page(item_page&& other) = default;

        bool can_fit(size_t item_id);
        void store(size_t item_id);
    private:
        Rectangle page_dimensions_;
        size_t last_insert_row_;
        size_t last_insert_column_;
        std::vector<std::vector<size_t>> items_;

};
#endif