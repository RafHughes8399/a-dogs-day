#include "system.h"

void systems::rendering_system::render(int frame){
    DrawTextureRec(background_.get_texture(), view_frame_, Vector2{0.0f, 0.0f}, WHITE);

    // TODO cull against the view frame once bounds have a home - the old
    // predicate read entity->get_hitbox().get_box(), and the hitbox is moving
    // into collision_component.
    auto render_predicate = [](size_t entity_id) -> bool {
        (void) entity_id;
        return true;
    };

    for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
        render_layers_[layer].draw(render_predicate,
            Vector2{view_frame_.x, view_frame_.y}, frame);
    }
}
