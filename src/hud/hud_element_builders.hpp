#ifndef HUD_ELEMENT_BUILDERS_H
#define HUD_ELEMENT_BUILDERS_H

#include "raylib.h"
#include "config.h"
#include <cstddef>

#include "sprite.h"
#include "hitbox.h"
#include "component.h"

namespace button_builders {
    void build_button(size_t id, Vector2 position, sprite::sprite& sprite, hitbox::hitbox& hitbox,
        size_t action, components::click_payload payload = {});
    void build_decoration_button(size_t id, Vector2 position, sprite::sprite& sprite, size_t action);
    void build_move_decoration_button(size_t id, Vector2 position);
    void build_sell_decoration_button(size_t id, Vector2 position);
    void build_to_storage_decoration_button(size_t id, Vector2 position);
    void build_recipe_button(size_t id, Vector2 position, size_t recipe, sprite::sprite& sprite);
    void build_recipe_button(size_t food, size_t id, Vector2 position, size_t recipe);
    void build_lasagna_button(size_t id, Vector2 position, size_t recipe);
    void build_bone_biscuits_button(size_t id, Vector2 position, size_t recipe);
    void build_espresso_button(size_t id, Vector2 position, size_t recipe);
    void build_pup_cup_button(size_t id, Vector2 position, size_t recipe);
    void build_cutlets_button(size_t id, Vector2 position, size_t recipe);
    void build_mutton_button(size_t id, Vector2 position, size_t recipe);
    void build_muttoff_button(size_t id, Vector2 position, size_t recipe);
    void build_prozac_button(size_t id, Vector2 position, size_t recipe);
    void build_escargot_button(size_t id, Vector2 position, size_t recipe);
    void build_boiled_chicken_button(size_t id, Vector2 position, size_t recipe);
    void build_risotto_button(size_t id, Vector2 position, size_t recipe);
    void build_carbonara_button(size_t id, Vector2 position, size_t recipe);
    void build_bird_button(size_t id, Vector2 position, size_t recipe);
    void build_sausage_on_a_stick_button(size_t id, Vector2 position, size_t recipe);
    void build_pork_knuckles_button(size_t id, Vector2 position, size_t recipe);
    void build_lemongrass_button(size_t id, Vector2 position, size_t recipe);
    void build_charpoocherie_board_button(size_t id, Vector2 position, size_t recipe);
    void build_cheddar_button(size_t id, Vector2 position, size_t recipe);
    void build_parmigiano_button(size_t id, Vector2 position, size_t recipe);
    void build_shishamo_button(size_t id, Vector2 position, size_t recipe);
    void build_rabbit_foot_button(size_t id, Vector2 position, size_t recipe);
    void build_brisket_leftovers_button(size_t id, Vector2 position, size_t recipe);
    void build_steak_leftovers_button(size_t id, Vector2 position, size_t recipe);
    void build_page_navigation_button(size_t id, Vector2 position, int direction);

}
#endif
