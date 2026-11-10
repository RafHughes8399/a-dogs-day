#ifndef PAGE_NAVIGATOR_H
#define PAGE_NAVIGATOR_H

#include "raylib.h"
#include "raymath.h"
#define SPACING {16.0f, 0.0f}
class page_navigator{
    public:
        ~page_navigator() = default;
        page_navigator(size_t left_button, size_t right_button, size_t page_num, size_t num_pages, float scale)
        : left_button_id_(left_button), right_button_id_(right_button), num_pages_(num_pages), page_num_(page_num), spacing_(Vector2Scale(SPACING, scale)){}
        page_navigator(const page_navigator& other) = default;
        page_navigator(page_navigator&& other) = default;
        
        page_navigator& operator=(const page_navigator& other) = default;
        page_navigator& operator=(page_navigator&& other) = default;

        void add_page();
        void remove_page();

    private:
        size_t left_button_id_;
        size_t right_button_id_;
        
        size_t num_pages_;
        size_t page_num_;

        Vector2 spacing_;
};
#endif
