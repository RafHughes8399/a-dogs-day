#ifndef HUD_LAYOUT_H
#define HUD_LAYOUT_H

#include <algorithm>
#include <cstddef>

#include "raylib.h"
#include "config.h"

namespace hud_layout {
    inline Rectangle decoration_panel(const Rectangle& hitbox){
        float height = std::max(hitbox.height * hud_config::decoration_hud_height_ratio,
            hud_config::decoration_hud_min_height);
        return Rectangle{
            hitbox.x - hud_config::decoration_hud_edge_gap - hud_config::decoration_hud_width,
            hitbox.y + (hitbox.height - height) / 2.0f,
            hud_config::decoration_hud_width,
            height};
    }
    inline Vector2 decoration_button_position(const Rectangle& panel, size_t index){
        const float size = hud_config::decoration_hud_button_size;
        const float count = static_cast<float>(hud_config::decoration_hud_button_count);
        const float buffer = (panel.height - count * size) / (count + 1.0f);
        return Vector2{
            panel.x + (panel.width - size) / 2.0f,
            panel.y + buffer + static_cast<float>(index) * (size + buffer)};
    }
}
#endif
