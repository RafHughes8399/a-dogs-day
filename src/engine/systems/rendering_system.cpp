#include "component.h"
#include "system.h"
#include <algorithm>


// ---------------- helpers ----------------
bool systems::rendering_system::is_entity_in_frame(size_t id, Rectangle view_frame){
    auto* collision = component_managers::collision_manager_.get_component(id);
    // nothing to cull against - draw it rather than hide it
    if(collision == nullptr){ return true; }

    auto entity_box = collision->get_hitbox_component().get_hitbox().get_box();
    return CheckCollisionRecs(view_frame, entity_box);
}

// ---------------- event handlers ----------------
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

// ---------------- render and teardown ----------------
void systems::rendering_system::render(int frame){
    // TODO cull against the view frame once hitbox bounds land in
    // collision_component
    auto render_predicate = [this](size_t entity_id) -> bool {
        return is_entity_in_frame(entity_id, view_frame_);
    };

    for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
        render_layers_[layer].draw(render_predicate,
            Vector2{view_frame_.x, view_frame_.y}, frame, true);
    }
    movement_system::get_instance().render_graph(view_frame_);
}

void systems::rendering_system::clear(){
    for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
        render_layers_[layer].clear();
    }
    view_frame_ = Rectangle{0.0f, 0.0f, level_config::screen_width, level_config::screen_height};
}

// ---------------- accessors  and modifiers ----------------
void systems::rendering_system::move_frame(Vector2 move_delta){
    float min = 0.0f;
    float max_x = std::max(level_config::world_x - view_frame_.width, min);
    float max_y = std::max(level_config::world_y - view_frame_.height, min);
    view_frame_.x = std::max(std::min(view_frame_.x + move_delta.x, max_x), min);
    view_frame_.y = std::max(std::min(view_frame_.y + move_delta.y, max_y), min);
}