#include "component.h"
#include "debug_log_interface.h"
#include <string>

namespace {
    Rectangle inventory_content_frame(){
        auto frame = hud_config::inventory_page_frame;
        auto inset = hud_config::page_nav_button_buffer;
        auto nav_strip = hud_config::page_nav_button_size + hud_config::page_nav_button_buffer;
        return Rectangle{inset, inset,
            frame.width - 2.0f * inset, frame.height - 2.0f * inset - nav_strip};
    }
}
size_t components::inventory_storage_component::size() const{
    return placements_.size();
}
bool components::inventory_storage_component::empty() const{
    return size() == 0;
}

std::optional<Vector2> components::inventory_storage_component::place(size_t definition_id, Vector2 footprint){
    auto content = inventory_content_frame();
    auto gap = hud_config::inventory_item_buffer;
    if(footprint.x > content.width or footprint.y > content.height){
        debug::log("[inventory_storage_component::place] footprint " + std::to_string(footprint.x) + " x "
            + std::to_string(footprint.y) + " larger than content " + std::to_string(content.width)
            + " x " + std::to_string(content.height));
        return std::nullopt;
    }
    auto wrapped = cursor_x_ + footprint.x > content.x + content.width;
    auto x = wrapped ? content.x : cursor_x_;
    auto y = wrapped ? row_y_ + row_height_ + gap : row_y_;
    debug::log("[inventory_storage_component::place] items: " + std::to_string(placements_.size())
        + ", cursor_x: " + std::to_string(cursor_x_) + ", row_y: " + std::to_string(row_y_)
        + ", row_height: " + std::to_string(row_height_) + ", wrapped: " + std::to_string(wrapped)
        + ", candidate: " + std::to_string(x) + ", " + std::to_string(y)
        + ", bottom needed: " + std::to_string(y + footprint.y)
        + ", content bottom: " + std::to_string(content.y + content.height));
    if(y + footprint.y > content.y + content.height){
        debug::log("[inventory_storage_component::place] page full, no vertical room");
        return std::nullopt;
    }
    cursor_x_ = x + footprint.x + gap;
    row_y_ = y;
    row_height_ = wrapped ? footprint.y : std::max(row_height_, footprint.y);
    placements_.emplace_back(definition_id, Rectangle{x, y, footprint.x, footprint.y});
    return Vector2{x, y};
}

// it takes in a desired item to take the not the dictioanry id of the item but the physical id
// what it does return is the dictionary item
size_t components::inventory_storage_component::take_item(size_t item_id){
    // TODO
    return 0;
}