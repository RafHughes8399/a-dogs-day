/** events between the ECS systems.
 *
 * everything in here is executed immediately, never queued - a system that
 * hears about an entity has to see it in the same frame it was created, not the
 * next one.
 */
#ifndef EVENTS_SYSTEM_EVENTS_H
#define EVENTS_SYSTEM_EVENTS_H

#include <optional>
#include <vector>

#include "event_core.h"
#include "path.h"
#include "raylib.h"

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
	class create_path_to : public event{
		public:
			// * checkpoints become legs of their own - src, dst, [c1, c2] queues
			// * src->c1, c1->c2, c2->dst. they are a route the caller wants
			// * taken, not a requirement: a leg that still spans zones splits
			// * again on its own
			create_path_to(size_t id, Vector2 destination,
				path::assignment mode = path::replace,
				std::optional<size_t> destination_entity = std::nullopt,
				std::vector<Vector2> checkpoints = {})
			: event(ids::create_path_to_id), id_(id), destination_(destination),
			mode_(mode), destination_entity_(destination_entity),
			checkpoints_(std::move(checkpoints)){}

			static int get_static_type(){
				return ids::create_path_to_id;
			}
			size_t get_id() const{
				return id_;
			}
			Vector2 get_destination() const{
				return destination_;
			}
			path::assignment get_assignment() const{
				return mode_;
			}
			std::optional<size_t> get_destination_entity() const{
				return destination_entity_;
			}
			const std::vector<Vector2>& get_checkpoints() const{
				return checkpoints_;
			}
		private:
			const size_t id_;
			const Vector2 destination_;
			const path::assignment mode_;
			const std::optional<size_t> destination_entity_;
			const std::vector<Vector2> checkpoints_;
	};
}

#endif
