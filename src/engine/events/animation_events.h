/** events raised by the animation system.
 *
 * queued, not executed - a finished animation is a fact the next frame acts on,
 * and queueing lands it in process_events before any system ticks rather than
 * part way through animation_system::update.
 */
#ifndef EVENTS_ANIMATION_EVENTS_H
#define EVENTS_ANIMATION_EVENTS_H

#include "event_core.h"

namespace events{
	// fact: a non-repeating animation played out its last frame. repeating
	// animations never raise this - they end when whoever started them calls stop
	class animation_finished : public event{
		public:
			animation_finished(size_t id, size_t sprite_slot, size_t animation_index)
			: event(ids::animation_finished_id), id_(id), sprite_slot_(sprite_slot),
			animation_index_(animation_index){}

			static int get_static_type(){
				return ids::animation_finished_id;
			}
			size_t get_id() const{
				return id_;
			}
			size_t get_sprite_slot() const{
				return sprite_slot_;
			}
			size_t get_animation_index() const{
				return animation_index_;
			}
		private:
			const size_t id_;
			const size_t sprite_slot_;
			const size_t animation_index_;
	};
}
#endif
