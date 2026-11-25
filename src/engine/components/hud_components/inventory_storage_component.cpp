#include "component.h"
#include <algorithm>
#include "debug_log_interface.h"
#include "game_config.h"
#include "hud_config.h"
#include "items.h"
#include <raylib.h>
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
    size_t occupied = 0;
    for(auto& row : slots_){
        occupied += std::count_if(row.begin(), row.end(), [](const placement& slot) -> bool {
            return slot.get_button() != static_cast<size_t>(game_config::empty_entity);
        });
    }
    return occupied;
}
bool components::inventory_storage_component::empty() const{
    return size() == 0;
}
void components::inventory_storage_component::init_empty_slots(float row_start, float row_end, float column_start, float column_end){
    auto slot = hud_config::inventory_item_slot_dimensions;
    auto rows = static_cast<int>((row_end - row_start) / slot.height);
    auto columns = static_cast<int>((column_end - column_start) / slot.width);
    for(int row = 0; row < rows; ++row){
        slots_.emplace_back();
        for(int column = 0; column < columns; ++column){
            slots_[row].emplace_back(game_config::empty_entity, Rectangle{column_start + column * slot.width, row_start + row * slot.height, slot.width, slot.height});
        }
    }
    next_free_slot_ = {0,0};
}
bool components::inventory_storage_component::can_place(){
    // check the free slots
    if(not freed_slots_.empty()){
        return true;
    }
    else{
        auto rows = slots_.size();
        auto columns = slots_[0].size();   
        return next_free_slot_.x < columns and next_free_slot_.y < rows;
    }
}
Vector2 components::inventory_storage_component::place_at(const items::item_ref& item_reference){
    // get the base position
    Vector2 slot_index;
    if(not freed_slots_.empty()){
        slot_index = freed_slots_.front();
        freed_slots_.pop();
    }
    else{
        slot_index = next_free_slot_;
        next_free_slot_.x += 1.0f;
        if(next_free_slot_.x >= static_cast<float>(slots_[0].size())){
            next_free_slot_.x = 0.0f;
            next_free_slot_.y += 1.0f;
        }
    }
    last_placed_ = slot_index;
    auto base_position = slots_[static_cast<size_t>(slot_index.y)][static_cast<size_t>(slot_index.x)].get_position();
    // then do the placement maths
    auto size = items::item_encyclopedia::get_instance().definition_of(item_reference).get_size();
    // compare width and height, scale as needed
    auto scale = hud_config::inventory_item_fit / std::max(size.x, size.y);
    auto scaled_width = size.x * scale;
    auto scaled_height = size.y * scale;
    // then centre the position of the button
    auto slot = hud_config::inventory_item_slot_dimensions;
    return Vector2{base_position.x + (slot.width - scaled_width) * 0.5f, base_position.y + (slot.height - scaled_height) * 0.5f};
}

void components::inventory_storage_component::attach_button(size_t button){
    slots_[static_cast<size_t>(last_placed_.y)][static_cast<size_t>(last_placed_.x)].set_button(button);
}
bool components::inventory_storage_component::take_item(size_t button){
    if(button == static_cast<size_t>(game_config::empty_entity)){ return false; }
    for(size_t row = 0; row < slots_.size(); ++row){
        for(size_t column = 0; column < slots_[row].size(); ++column){
            auto& slot = slots_[row][column];
            if(slot.get_button() != button){ continue; }
            freed_slots_.push(Vector2{static_cast<float>(column), static_cast<float>(row)});
            slot.set_button(static_cast<size_t>(game_config::empty_entity));
            return true;
        }
    }
    return false;
}
