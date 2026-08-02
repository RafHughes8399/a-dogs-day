/** events between the ECS systems.
 *
 * everything in here is executed immediately, never queued - a system that
 * hears about an entity has to see it in the same frame it was created, not the
 * next one.
 */
#ifndef EVENTS_SYSTEM_EVENTS_H
#define EVENTS_SYSTEM_EVENTS_H

#include "event_core.h"

namespace events{
	// fact: the entity exists and its components are registered
	class create_entity : public event{
		public:
			create_entity(size_t id, size_t layer)
			: event(ids::create), id_(id), layer_(layer){}

			static int get_static_type(){
				return ids::create;
			}
			size_t get_id() const{
				return id_;
			}
			size_t get_layer() const{
				return layer_;
			}
		private:
			const size_t id_;
			const size_t layer_;
	};
}

#endif
