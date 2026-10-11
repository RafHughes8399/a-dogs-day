#ifndef HUD_ELEMENT_BUILDERS_H
#define HUD_ELEMENT_BUILDERS_H

#include "raylib.h"
#include "config.h"
#include <cstddef>

#include "sprite.h"
#include "hitbox.h"

namespace button_builders {
    void build_button(size_t id, Vector2 position, sprite::sprite& sprite, hitbox::hitbox& hitbox);
    void build_cook_station_recipe_button(size_t id, Vector2 position);
    void build_page_navigation_button(size_t id, Vector2 position, int direction);

}
#endif