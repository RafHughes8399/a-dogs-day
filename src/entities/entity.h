/**
 * base entity class + shared entity includes.
 * Every entity subclass header includes this one, so the common third-party
 * and engine includes live here.
 */
#ifndef ENTITY_H
#define ENTITY_H

#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "sprite.h"
#include "raylib.h"
#include "body.h"
#include "debug_log_interface.h"
#include "query_interface.h"
#include "queries.h"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <queue>

namespace entities{
    // Flags, not alternatives - one update can report several. Test with &.
    enum status_codes{
        nothing        = 0,
        moved          = 1 << 0,
        dead           = 1 << 1,
        completed_path = 1 << 2
    };
    // ------------------------- entities ------------------------- //
    class entity {
        public:
            virtual ~entity() = default;
            entity(body::body body, Vector2 position, int id, std::string debug_id)
            : body_(body), debug_id_(std::move(debug_id)), id_(id), position_(position){}
            entity(const entity& other) = default;
            entity(entity&& other) = default;

            entity& operator=(const entity& other) = delete;
            entity& operator=(entity&& other) = delete;

            bool operator==(entity& other){
                return id_ == other.id_;
            }

            bool check_collision(const hitbox::hitbox other);
            body::body& get_body();
            const std::string& get_debug_id();
            hitbox::hitbox& get_hitbox();
            int get_id();
            Vector2 get_position();
            sprite::sprite& get_sprite();

            virtual void interact(entity& other){
                (void) other;
                return;
            }

            void move(Vector2 new_postion);
            void move_without_event(Vector2 new_position);

            virtual void render(Vector2 draw_position, int frame);

            virtual int update(float delta, int frame){
                (void) delta;
                (void) frame;
                return status_codes::nothing;
            }

        protected:
            body::body body_;
            const std::string debug_id_;
            const int id_;
            Vector2 position_;

    };
}
#endif
