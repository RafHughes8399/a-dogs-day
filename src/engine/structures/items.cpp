#include "items.h"
#include "hud_config.h"
#include <algorithm>
#include <array>
#include <cmath>

size_t items::food_item::get_id(){
    return item_id_;
}
std::uint8_t items::food_item::get_count() const{
    return count_;
}
void items::food_item::adjust_count(size_t delta){
    count_ += delta;
}

const std::string& items::item_definition::get_name() const{
    return name_;
}
int items::item_definition::get_price() const{
    return price_;
}
Vector2 items::item_definition::get_size() const{
    return size_;
}

std::array<items::item_definition, entity_config::decorations_size> items::item_encyclopedia::build_definitions(){
    return std::array<item_definition, entity_config::decorations_size>{
        item_definition("Test Decoration", 10, Vector2{
            entity_config::test_decoration_attributes[entity_config::attributes::frame_width],
            entity_config::test_decoration_attributes[entity_config::attributes::frame_height]}),
        item_definition("Conversation Pit", 10, Vector2{
            entity_config::conversation_pit_attributes[entity_config::attributes::frame_width],
            entity_config::conversation_pit_attributes[entity_config::attributes::frame_height]}),
        item_definition("Gargoyle Void", 10, Vector2{
            entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width],
            entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height]}),
        item_definition("Pavlov", 10, Vector2{
            entity_config::pavlov_attributes[entity_config::attributes::frame_width],
            entity_config::pavlov_attributes[entity_config::attributes::frame_height]}),
        item_definition("Polish Cow", 10, Vector2{
            entity_config::polish_cow_attributes[entity_config::attributes::frame_width],
            entity_config::polish_cow_attributes[entity_config::attributes::frame_height]}),
        item_definition("Dog Poker", 10, Vector2{
            entity_config::poker_table_attributes[entity_config::attributes::frame_width],
            entity_config::poker_table_attributes[entity_config::attributes::frame_height]}),
        item_definition("One Dog Goes This Way", 10, Vector2{
            entity_config::dog_painting_attributes[entity_config::attributes::frame_width],
            entity_config::dog_painting_attributes[entity_config::attributes::frame_height]}),
        item_definition("Ficus Lytra", 10, Vector2{
            entity_config::ficus_lytra_attributes[entity_config::attributes::frame_width],
            entity_config::ficus_lytra_attributes[entity_config::attributes::frame_height]}),
        item_definition("Frangipani", 10, Vector2{
            entity_config::frangipani_attributes[entity_config::attributes::frame_width],
            entity_config::frangipani_attributes[entity_config::attributes::frame_height]})
    };
}
const items::item_definition& items::item_encyclopedia::definition_of(size_t definition_id) const{
    return definitions_.at(definition_id);
}
Vector2 items::inventory_layout::get_size() const{
    return size_;
}
float items::inventory_layout::get_scale() const{
    return scale_;
}
Vector2 items::inventory_layout::get_footprint() const{
    return Vector2{size_.x + hud_config::inventory_item_border, size_.y + hud_config::inventory_item_border};
}
items::inventory_layout items::inventory_layout_of(size_t definition_id){
    auto size = item_encyclopedia::get_instance().definition_of(definition_id).get_size();
    auto dominant = std::max(size.x, size.y);
    auto t = std::clamp((dominant - hud_config::inventory_scale_threshold)
        / (hud_config::inventory_scale_reference - hud_config::inventory_scale_threshold), 0.0f, 1.0f);
    auto eased = t * t * (3.0f - 2.0f * t);
    auto scale = dominant <= hud_config::inventory_scale_threshold ? 1.0f
        : std::lerp(hud_config::inventory_scale_max, hud_config::inventory_scale_min, eased);
    return inventory_layout(Vector2{size.x * scale, size.y * scale}, scale);
}
