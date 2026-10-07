#include "component.h"
#include "config.h"
#include "system.h"
#include <algorithm>
#include <raylib.h>
#include <raymath.h>


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


    // * the predicate allows for partial rendering already it seems
    auto render_predicate = [this](size_t entity_id) -> bool {
        return is_entity_in_frame(entity_id, view_frame_);
    };

    for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
        render_layers_[layer].draw(render_predicate,
            Vector2{view_frame_.x, view_frame_.y}, frame, true);
    }
    //movement_system::get_instance().render_graph(view_frame_);
    Rectangle queue = {0, -2* level_config::edge_weight, level_config::edge_weight * 4.5f, level_config::world_y + (4 * level_config::edge_weight)};
    DrawRectangleLines(static_cast<int>(queue.x), static_cast<int>(queue.y), static_cast<int>(queue.width), static_cast<int>(queue.height), RED);
}

void systems::rendering_system::clear(){
    for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
        render_layers_[layer].clear();
    }
    view_frame_ = Rectangle{0.0f, 0.0f, level_config::screen_width, level_config::screen_height};
}

// ---------------- accessors  and modifiers ----------------
void systems::rendering_system::recalibrate_view_frame(){
    // Convert top-left and bottom-right screen corners to world space
    Vector2 topLeft = GetScreenToWorld2D(Vector2Zero(), camera_);
    Vector2 bottomRight = GetScreenToWorld2D({level_config::screen_width, level_config::screen_height}, camera_);
    view_frame_ = {  topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y };
}
void systems::rendering_system::move_camera(Vector2 move_delta){
    // TODO this needs to be rewritten, clamped again to the mins and maxes, adjust the target but retain
    // the offset.
    camera_.target.x = Clamp(camera_.target.x + move_delta.x, 0.0f, level_config::world_x - camera_.offset.x);
    camera_.target.y = Clamp(camera_.target.y + move_delta.y, 0.0f, level_config::world_y - camera_.offset.y);
    recalibrate_view_frame();
}
void systems::rendering_system::adjust_zoom(float zoom_delta){
    camera_.zoom = Clamp(camera_.zoom + zoom_delta, camera_config::zoom_min,  camera_config::zoom_max);
    recalibrate_view_frame();
}
Vector2 systems::rendering_system::screen_to_world(Vector2 screen_position){
    auto clamped = Vector2Clamp(screen_position, Vector2Zero(),
        Vector2{level_config::screen_width, level_config::screen_height});
    return Vector2Add(clamped, Vector2{view_frame_.x, view_frame_.y});
}
