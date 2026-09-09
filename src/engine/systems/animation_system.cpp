#include "system.h"

void systems::animation_system::play(size_t entity, const std::vector<sprite_animation>& animations){
    auto* renderable = component_managers::renderable_manager_.get_component(entity);
    if(renderable == nullptr){ return; }

    for(const auto& anim : animations){
        auto* slot = renderable->get_sprite_layer(anim.sprite_slot);
        if(slot == nullptr or slot->get_sprites().empty()){ continue; }

        for(auto& slot_sprite : slot->get_sprites()){
            slot_sprite.set_animation(anim.animation_index);
            slot_sprite.get_animation().play(anim.repeat);
        }
        // a new play on a slot supersedes whatever was counting down on it
        std::erase_if(current_animations_, [entity, &anim](const auto& tracked) -> bool {
            return tracked.entity == entity and tracked.sprite_slot == anim.sprite_slot;
        });
        if(anim.repeat){ continue; }

        auto& played = slot->get_sprites().front().get_animation();
        current_animations_.push_back(animation{entity, anim.sprite_slot, anim.animation_index,
            played.num_frames() * played.get_play_speed()});
    }
}

void systems::animation_system::play(size_t entity, sprite_animation animation){
    play(entity, std::vector<sprite_animation>{animation});
}

void systems::animation_system::stop(size_t entity){
    auto* renderable = component_managers::renderable_manager_.get_component(entity);
    if(renderable == nullptr){ return; }

    for(auto& slot : renderable->get_layers()){
        for(auto& slot_sprite : slot.get_sprites()){
            slot_sprite.get_animation().pause();
        }
    }
    std::erase_if(current_animations_, [entity](const auto& tracked) -> bool {
        return tracked.entity == entity;
    });
}

void systems::animation_system::stop(size_t entity, size_t sprite_slot){
    auto* renderable = component_managers::renderable_manager_.get_component(entity);
    if(renderable == nullptr){ return; }

    if(auto* slot = renderable->get_sprite_layer(sprite_slot)){
        for(auto& slot_sprite : slot->get_sprites()){
            slot_sprite.get_animation().pause();
        }
    }
    std::erase_if(current_animations_, [entity, sprite_slot](const auto& tracked) -> bool {
        return tracked.entity == entity and tracked.sprite_slot == sprite_slot;
    });
}

void systems::animation_system::on_destroyed_entity(const events::remove_entity& event){
    auto entity = event.get_id();
    std::erase_if(current_animations_, [entity](const auto& tracked) -> bool {
        return tracked.entity == entity;
    });
}

// delta is unused - the countdown is in frames, and the game loop ticks update
// once per rendered frame, the same cadence animation::advance steps on
void systems::animation_system::update(float delta){
    (void) delta;
    std::vector<animation> finished;

    for(auto& tracked : current_animations_){
        tracked.frames_remaining--;
        if(tracked.frames_remaining <= 0){
            finished.push_back(tracked);
        }
    }
    if(finished.empty()){ return; }

    std::erase_if(current_animations_, [](const auto& tracked) -> bool {
        return tracked.frames_remaining <= 0;
    });
    // erase first, then pause and announce - stop() prunes current_animations_ too, and a
    // handler reacting to the fact may start the next animation on this slot
    for(const auto& tracked : finished){
        stop(tracked.entity, tracked.sprite_slot);
        std::unique_ptr<events::event> event = std::make_unique<events::animation_finished>(
            tracked.entity, tracked.sprite_slot, tracked.animation_index);
        event_interface::queue_event(event);
    }
}
