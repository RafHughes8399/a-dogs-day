/** Generic entity lifecycle/movement events. */
#ifndef EVENTS_ENTITY_EVENTS_H
#define EVENTS_ENTITY_EVENTS_H

#include "event_core.h"
#include "hitbox.h"
namespace events{
	// when an entity moves, main listener is the quad tree to move entities into the correct node
	class move_entity : public event{
		public:
		move_entity(size_t id, Rectangle pre_move = Rectangle{}, Rectangle post_move = Rectangle{})
			: event(ids::move), id_(id), pre_move_(pre_move), post_move_(post_move){}
		static int get_static_type(){
			return ids::move;
		}
		size_t get_id() const{
			return id_;
		}
		Rectangle get_pre_move() const{
			return pre_move_;
		}
		Rectangle get_post_move() const{
			return post_move_;
		}
		private:
		const size_t id_;
		const Rectangle pre_move_;
		const Rectangle post_move_;
	};
	// for when an entity need be removed from the level, main listener is the quad tree
	class remove_entity : public event{
		public:
			remove_entity(size_t id)
			: event(ids::remove), id_(id){}

			static int get_static_type(){
				return ids::remove;
			}
			size_t get_id() const{
				return id_;
			}
		private:
			const size_t id_;

	};
}

#endif
