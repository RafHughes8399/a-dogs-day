#include "component_helpers.hpp"
#include "hitbox.h"
#include "hud_element_builders.hpp"
#include "sprite.h"

void button_builders::build_button(size_t id, Vector2 position, sprite::sprite& sprite, hitbox::hitbox& hitbox){
    // positional component
    component_helpers::add_hud_positional_component(id, position);
    // renderable component
    //component_helpers::add_hud_renderable_component(id, std::vector<components::renderable_component::body> &bodys)
    // state component [hidden, active, hovered]

    // clickable component

    // collision component

}