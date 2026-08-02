#include "system.h"

void systems::rendering_system::on_created_entity(const events::create_entity& event){
    auto layer = event.get_layer();
    if(layer >= level_config::draw_layers::size){ return; }
    render_layers_[layer].add_entity(event.get_id());
}

void systems::rendering_system::on_destroyed_entity(const events::remove_entity& event){
    // the layer is not carried on removal, so drop the id from all of them
    for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
        render_layers_[layer].remove_entity(event.get_id());
    }
}

void systems::rendering_system::render(int frame){
    DrawTextureRec(background_.get_texture(), view_frame_, Vector2{0.0f, 0.0f}, WHITE);

    // TODO cull against the view frame once hitbox bounds land in
    // collision_component
    auto render_predicate = [](size_t entity_id) -> bool {
        (void) entity_id;
        return true;
    };

    for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
        render_layers_[layer].draw(render_predicate,
            Vector2{view_frame_.x, view_frame_.y}, frame);
    }
}
