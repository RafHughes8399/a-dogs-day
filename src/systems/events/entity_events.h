/** Generic entity lifecycle/movement events, plus station (table/food
 * counter) registration facts.
 */
#ifndef EVENTS_ENTITY_EVENTS_H
#define EVENTS_ENTITY_EVENTS_H

#include "event_core.h"

namespace events{
	// when an entity moves, main listener is the quad tree to move entities into the correct node
	class move_entity : public event{
		public:
		move_entity(size_t id)
			: event(ids::move), id_(id){}
		static int get_static_type(){
			return ids::move;
		}
		size_t get_id() const{
			return id_;
		}
		private:
		const size_t id_;
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
	// Cafe-domain fact: a table entity exists in the level and can be tracked by
	// the maitre d'. The event carries value data so the cafe/order system does
	// not need concrete entity types or ownership of level entities.
	class registered_table : public event{
		public:
			registered_table(entities::table* table)
			: event(ids::register_table), table_(table){}

			static int get_static_type(){
				return ids::register_table;
			}
			entities::table* get_table() const{
				return table_;
			}
		private:
			entities::table* const table_;
	};
	class removed_table : public event{
		public:
			removed_table(size_t table_id)
			: event(ids::table_removed), table_id_(table_id){}

			static int get_static_type(){
				return ids::table_removed;
			}
			size_t get_table_id() const{
				return table_id_;
			}
		private:
			const size_t table_id_;
	};
	// Cafe-domain fact: a food counter/pickup point exists. The expediter uses
	// these positions when routing waiter dogs through a pickup checkpoint.
	class registered_food_counter : public event{
		public:
			registered_food_counter(entities::food_counter* counter)
			: event(ids::register_food_counter), counter_(counter){}

			static int get_static_type(){
				return ids::register_food_counter;
			}
			entities::food_counter* get_counter() const{
				return counter_;
			}
		private:
			entities::food_counter* const counter_;
	};
	// Cafe-domain fact: a food counter was removed from the level; the expediter
	// must drop its pointer to avoid dereferencing a destroyed entity.
	class removed_food_counter : public event{
		public:
			removed_food_counter(size_t counter_id)
			: event(ids::food_counter_removed), counter_id_(counter_id){}

			static int get_static_type(){
				return ids::food_counter_removed;
			}
			size_t get_counter_id() const{
				return counter_id_;
			}
		private:
			const size_t counter_id_;
	};
}

#endif
