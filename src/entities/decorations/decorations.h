/**
 * decoration entity. Base class for placeable furniture (and stations).
 * body behaves slightly differently for decorations: it holds the variants for
 * the decoration.
 */
#ifndef DECORATIONS_H
#define DECORATIONS_H

#include "entity.h"

namespace entities{
    class decoration : public entity {
        public:
            decoration(body::body body, Vector2 position, int id, std::string debug_id)
            : entity(body, position, id, std::move(debug_id)),
            moved_cursor_handler([this](const events::moved_cursor& event) -> void { on_moved_cursor(event);} ),
            post_move_position_(position_), pre_move_position_(position_){
                // upon creating a decoration, let the graph know where it was placed with the event
                auto rectangle = body_.get_hitbox().get_box();
                std::unique_ptr<events::event> place_decoration = std::make_unique<events::placed_decoration>(rectangle, id_);
                event_interface::execute_event(*place_decoration);
            }
            decoration(const decoration& other) = default;
            decoration(decoration&& other) = default;

            decoration& operator=(const decoration& other) = delete;
            decoration& operator=(decoration&& other) = delete;

            bool can_place_down();
            void on_moved_cursor(const events::moved_cursor& event);
            void pick_up();
            virtual void place_down();
            void subscribe_to_cursor();
            void unsubscribe_from_cursor();

        private:
            Vector2 round_position();
            events::event_handler<events::moved_cursor> moved_cursor_handler;
            Vector2 post_move_position_;
            Vector2 pre_move_position_;

    };
}
#endif
