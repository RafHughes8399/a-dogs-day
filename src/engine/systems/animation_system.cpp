#include "system.h"

void systems::animation_system::play(size_t entity, const std::vector<sprite_animation>& animations){
    auto* renderable = component_managers::renderable_manager_.get_component(entity);
    if(renderable == nullptr){ return; }

    for(const auto& anim : animations){
        auto* slot = renderable->get_sprite_component(anim.sprite_slot);
        if(slot == nullptr){ continue; }

        for(auto& slot_sprite : slot->get_sprites()){
            slot_sprite.get_animation().goto_animation(static_cast<int>(anim.animation_index));
            slot_sprite.get_animation().play(anim.repeat);
        }
    }
}

void systems::animation_system::play(size_t entity, sprite_animation animation){
    play(entity, std::vector<sprite_animation>{animation});
}

void systems::animation_system::stop(size_t entity){
    auto* renderable = component_managers::renderable_manager_.get_component(entity);
    if(renderable == nullptr){ return; }

    for(auto& slot : renderable->get_sprites()){
        for(auto& slot_sprite : slot.get_sprites()){
            slot_sprite.get_animation().pause();
        }
    }
}

void systems::animation_system::stop(size_t entity, size_t sprite_slot){
    auto* renderable = component_managers::renderable_manager_.get_component(entity);
    if(renderable == nullptr){ return; }

    auto* slot = renderable->get_sprite_component(sprite_slot);
    if(slot == nullptr){ return; }

    for(auto& slot_sprite : slot->get_sprites()){
        slot_sprite.get_animation().pause();
    }
}
