#include "component_helpers.hpp"
#include "hitbox.h"
#include "hud_element_builders.hpp"
#include "sprite.h"

void button_builders::build_button(size_t id, Vector2 position, sprite::sprite& sprite, hitbox::hitbox& hitbox,
    size_t action, int payload){
    component_helpers::add_hud_positional_component(id, position);

    std::vector<sprite::sprite> sprites{sprite};
    std::vector<components::renderable_component::body> bodys{component_builders::build_hud_body(sprites, 0)};
    component_helpers::add_hud_renderable_component(id, bodys);

    std::vector<hitbox::hitbox> hitboxes{hitbox};
    component_helpers::add_hud_collision_component(id,
        component_builders::build_hud_hitbox_component(hitboxes, 0));

    component_helpers::add_hud_clickable_component(id, action, payload);
}
void button_builders::build_recipe_button(size_t id, Vector2 position, int recipe, sprite::sprite& sprite){
    auto hitbox = hitbox::hitbox(Rectangle{position.x, position.y,
        hud_config::recipe_icon_width, hud_config::recipe_icon_height});
    build_button(id, position, sprite, hitbox, hud_config::click_actions::select_recipe, recipe);
}
void button_builders::build_recipe_button(size_t food, size_t id, Vector2 position, int recipe){
    switch(food){
        case entity_config::foods::lasagna: build_lasagna_button(id, position, recipe); return;
        case entity_config::foods::bone_biscuits: build_bone_biscuits_button(id, position, recipe); return;
        case entity_config::foods::espresso: build_espresso_button(id, position, recipe); return;
        case entity_config::foods::pup_cup: build_pup_cup_button(id, position, recipe); return;
        case entity_config::foods::cutlets: build_cutlets_button(id, position, recipe); return;
        case entity_config::foods::boiled_chicken: build_boiled_chicken_button(id, position, recipe); return;
        case entity_config::foods::risotto: build_risotto_button(id, position, recipe); return;
        case entity_config::foods::carbonara: build_carbonara_button(id, position, recipe); return;
        case entity_config::foods::bird: build_bird_button(id, position, recipe); return;
        case entity_config::foods::sausage_on_a_stick: build_sausage_on_a_stick_button(id, position, recipe); return;
        case entity_config::foods::pork_knuckles: build_pork_knuckles_button(id, position, recipe); return;
        case entity_config::foods::lemongrass: build_lemongrass_button(id, position, recipe); return;
        case entity_config::foods::charpoocherie_board: build_charpoocherie_board_button(id, position, recipe); return;
        case entity_config::foods::cheddar: build_cheddar_button(id, position, recipe); return;
        case entity_config::foods::parmigiano: build_parmigiano_button(id, position, recipe); return;
        case entity_config::foods::shishamo: build_shishamo_button(id, position, recipe); return;
        case entity_config::foods::rabbit_foot: build_rabbit_foot_button(id, position, recipe); return;
        case entity_config::foods::brisket_leftovers: build_brisket_leftovers_button(id, position, recipe); return;
        case entity_config::foods::steak_leftovers: build_steak_leftovers_button(id, position, recipe); return;
        default: return;
    }
}
void button_builders::build_lasagna_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_lasagna_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_bone_biscuits_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_espresso_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_espresso_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_pup_cup_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_cutlets_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_cutlets_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_boiled_chicken_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_risotto_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_carbonara_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_bird_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_sausage_on_a_stick_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_pork_knuckles_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_lemongrass_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_charpoocherie_board_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_cheddar_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_parmigiano_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_shishamo_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_rabbit_foot_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_brisket_leftovers_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_steak_leftovers_button(size_t id, Vector2 position, int recipe){
    auto sprite = sprite_builders::build_food_sprite();
    build_recipe_button(id, position, recipe, sprite);
}
void button_builders::build_decoration_button(size_t id, Vector2 position, sprite::sprite& sprite, size_t action){
    auto hitbox = hitbox::hitbox(Rectangle{position.x, position.y,
        hud_config::decoration_hud_button_size, hud_config::decoration_hud_button_size});
    build_button(id, position, sprite, hitbox, action);
}
void button_builders::build_move_decoration_button(size_t id, Vector2 position){
    auto sprite = sprite_builders::build_move_decoration_button_sprite();
    build_decoration_button(id, position, sprite, hud_config::click_actions::move_decoration);
}
void button_builders::build_sell_decoration_button(size_t id, Vector2 position){
    auto sprite = sprite_builders::build_sell_decoration_button_sprite();
    build_decoration_button(id, position, sprite, hud_config::click_actions::sell_decoration);
}
void button_builders::build_to_storage_decoration_button(size_t id, Vector2 position){
    auto sprite = sprite_builders::build_to_storage_decoration_button_sprite();
    build_decoration_button(id, position, sprite, hud_config::click_actions::store_decoration);
}
