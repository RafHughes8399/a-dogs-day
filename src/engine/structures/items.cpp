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
size_t items::item_definition::get_category() const{
    return category_;
}

std::vector<items::item_definition> items::item_encyclopedia::build_decoration_definitions(){
    return std::vector<item_definition>{
        item_definition("Test Decoration", 10, Vector2{
            entity_config::test_decoration_attributes[entity_config::attributes::frame_width],
            entity_config::test_decoration_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("Conversation Pit", 10, Vector2{
            entity_config::conversation_pit_attributes[entity_config::attributes::frame_width],
            entity_config::conversation_pit_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("Gargoyle Void", 10, Vector2{
            entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width],
            entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("Pavlov", 10, Vector2{
            entity_config::pavlov_attributes[entity_config::attributes::frame_width],
            entity_config::pavlov_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("Polish Cow", 10, Vector2{
            entity_config::polish_cow_attributes[entity_config::attributes::frame_width],
            entity_config::polish_cow_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("Dog Poker", 10, Vector2{
            entity_config::poker_table_attributes[entity_config::attributes::frame_width],
            entity_config::poker_table_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("One Dog Goes This Way", 10, Vector2{
            entity_config::dog_painting_attributes[entity_config::attributes::frame_width],
            entity_config::dog_painting_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("Ficus Lytra", 10, Vector2{
            entity_config::ficus_lytra_attributes[entity_config::attributes::frame_width],
            entity_config::ficus_lytra_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category),
        item_definition("Frangipani", 10, Vector2{
            entity_config::frangipani_attributes[entity_config::attributes::frame_width],
            entity_config::frangipani_attributes[entity_config::attributes::frame_height]}, entity_config::item_categories::decoration_category)
    };
}
std::vector<items::item_definition> items::item_encyclopedia::build_station_definitions(){
    const Vector2 footprint{entity_config::station_width, entity_config::station_height};
    const auto category = entity_config::item_categories::station_category;
    return std::vector<item_definition>{
        item_definition("Dining Table", 10, footprint, category),
        item_definition("Tiled Table", 10, footprint, category),
        item_definition("Test Table", 10, footprint, category),
        item_definition("Orange Tiled Table", 10, footprint, category),
        item_definition("Red Tiled Table", 10, footprint, category),
        item_definition("Green Tiled Table", 10, footprint, category),
        item_definition("Blue Tiled Table", 10, footprint, category),
        item_definition("Wooden Food Counter", 10, footprint, category),
        item_definition("Stove", 10, footprint, category),
        item_definition("Oven", 10, footprint, category),
        item_definition("Espresso Station", 10, footprint, category),
        item_definition("Mini Fridge", 10, footprint, category),
        item_definition("Bush", 10, footprint, category),
        item_definition("Chopping Board", 10, footprint, category)
    };
}
std::array<std::vector<items::item_definition>, entity_config::item_categories_size> items::item_encyclopedia::build_catalogues(){
    std::array<std::vector<item_definition>, entity_config::item_categories_size> catalogues;
    catalogues[entity_config::decoration_category] = build_decoration_definitions();
    catalogues[entity_config::station_category] = build_station_definitions();
    return catalogues;
}
const items::item_definition& items::item_encyclopedia::definition_of(const item_ref& ref) const{
    return definition_of(ref.get_category(), ref.get_id());
}
const items::item_definition& items::item_encyclopedia::definition_of(size_t category, size_t id) const{
    return catalogues_.at(category).at(id);
}
bool items::item_encyclopedia::has_definition(const item_ref& ref) const{
    return ref.get_category() < catalogues_.size() and ref.get_id() < catalogues_[ref.get_category()].size();
}
size_t items::item_ref::get_category() const{
    return category_;
}
size_t items::item_ref::get_id() const{
    return id_;
}
size_t items::item_ref::pack() const{
    return (category_ << id_bits) | id_;
}
items::item_ref items::item_ref::unpack(size_t packed){
    return item_ref(packed >> id_bits, packed & ((static_cast<size_t>(1) << id_bits) - 1));
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
items::inventory_layout items::inventory_layout_of(const item_ref& ref){
    auto size = item_encyclopedia::get_instance().definition_of(ref).get_size();
    auto dominant = std::max(size.x, size.y);
    auto t = std::clamp((dominant - hud_config::inventory_scale_threshold)
        / (hud_config::inventory_scale_reference - hud_config::inventory_scale_threshold), 0.0f, 1.0f);
    auto eased = t * t * (3.0f - 2.0f * t);
    auto scale = dominant <= hud_config::inventory_scale_threshold ? 1.0f
        : std::lerp(hud_config::inventory_scale_max, hud_config::inventory_scale_min, eased);
    return inventory_layout(Vector2{size.x * scale, size.y * scale}, scale);
}
