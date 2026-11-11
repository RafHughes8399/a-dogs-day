#include "entity.h"
#include "hud_element_builders.hpp"
#include "sprite.h"

void page_builders::build_inventory_page(size_t id, Rectangle frame){
    auto panel = sprite_builders::build_decoration_nine_sprite(Rectangle{0.0f, 0.0f, frame.width, frame.height});
    ecs_entities::build_hud_root(id, Vector2{frame.x, frame.y}, panel);
}
