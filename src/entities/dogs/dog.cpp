#include "entities.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include <vector>
// ------------------------------- dogs ------------------------------- //
bool entities::dog::reached_position(Vector2 target){
    float position_to_target = Vector2Distance(position_, target);
    if(position_to_target <= level_config::edge_weight * 0.05){
        position_ = target;
        return true;
    }
    return false;
}

int entities::dog::update(float delta, int frame){
    (void) frame;
    update_path(delta);
    return status_codes::nothing;
}

void entities::dog::update_path(float delta){
    if(current_path_.empty()){
        return;
    }

    auto next_position = current_path_.front();
    if(reached_position(next_position)){
        current_path_.erase(current_path_.begin());

        if(current_path_.empty()){
            debug::log(
                "[dog::update_path, completed current path] "
                "dog_id: " + std::to_string(id_)
                + ", destination: " + raglib::vector_to_string(next_position)
                + ", queued_paths: " + std::to_string(move_paths_.size()));
            on_path_finished(next_position);
            body_.update_hitboxes(position_);
            start_next_path();
            return;
        }

        determine_direction(current_path_.front());
    }

    move_toward_current_waypoint(delta);
}

void entities::dog::move_toward_current_waypoint(float delta){
    auto new_position = Vector2Add(position_, Vector2Scale(Vector2Multiply(move_speed_, direction_scalar_), delta));
    position_ = new_position;
    body_.update_hitboxes(position_);
}

void entities::dog::start_next_path(){
    if(move_paths_.empty()){
        return;
    }

    current_path_ = move_paths_.front();
    move_paths_.pop();
    determine_direction(current_path_.front());
    debug::log(
        "[dog::start_next_path, switched to queued path] "
        "dog_id: " + std::to_string(id_)
        + ", path_size: " + std::to_string(current_path_.size())
        + ", first_position: " + raglib::vector_to_string(current_path_.front())
        + ", queued_paths_remaining: " + std::to_string(move_paths_.size()));
}

Vector2 entities::dog::get_direction_scalar(){
    return direction_scalar_;
}
void entities::dog::set_direction_index(size_t direction){
    if(direction < body_.num_sprites()){
        body_.set_index(direction);
    }
    if(direction < head_.num_sprites()){
        head_.set_index(direction);
    }
}
void entities::dog::determine_direction(Vector2 target){
    if(position_.x < target.x){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::right];
        set_direction_index(level_config::directions::right);
        return;
    }
    else if(position_.x > target.x){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::left];
        set_direction_index(level_config::directions::left);
        return;
    }
    else if(position_.y < target.y){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::down];
        set_direction_index(level_config::directions::down);
        return;
    }
    else if(position_.y > target.y){
        direction_scalar_ = level_config::direction_scalars[level_config::directions::up];
        set_direction_index(level_config::directions::up);
        return;
    }
}

void entities::dog::render(Vector2 draw_position, int frame){
    entity::render(draw_position, frame);
    head_.render(draw_position, frame);
}

void entities::dog::set_path(const std::vector<Vector2>& path){
    if(path.empty()){
        debug::log(
            "[dog::set_path, skipped empty path] "
            "dog_id: " + std::to_string(id_));
        return;
    }
    if(current_path_.empty()){
        current_path_ = path;
        determine_direction(current_path_.front());
        debug::log(
            "[dog::set_path, assigned current path] "
            "dog_id: " + std::to_string(id_)
            + ", path_size: " + std::to_string(current_path_.size())
            + ", first_position: " + raglib::vector_to_string(current_path_.front())
            + ", queued_paths: " + std::to_string(move_paths_.size()));
    }
    else{
        auto next_path = path;
        next_path.front() = current_path_.back();
        move_paths_.push(next_path);
        debug::log(
            "[dog::set_path, queued next path] "
            "dog_id: " + std::to_string(id_)
            + ", path_size: " + std::to_string(next_path.size())
            + ", first_position: " + raglib::vector_to_string(next_path.front())
            + ", queued_paths: " + std::to_string(move_paths_.size()));

	}
}
void entities::dog::set_path(const std::vector<Vector2>& path, int furniture_id, Vector2 furniture_position){
    (void) furniture_id;
    (void) furniture_position;
    set_path(path);
}

void entities::dog::on_path_finished(Vector2 destination){
    std::unique_ptr<events::event> reached_destination = std::make_unique<events::dog_completed_path>(id_, destination);
    event_interface::queue_event(reached_destination);
}

// ------------------------------- npc dogs ------------------------------- //
int entities::npc_dog::update(float delta, int frame){
    return dog::update(delta, frame);
}
