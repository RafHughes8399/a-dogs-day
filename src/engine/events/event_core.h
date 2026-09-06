/** the core of the event system: the shared event id enum, the event base
 * class, generic/sentinel events, and the handler/dispatcher machinery that
 * every category-specific event header builds on.
 *
 * takes advantage of a obsever-listener pattern, enables classes and components within the codebase to
 * queue events with certain information upon something occuring (like an object moving) that are then executed either
 * immediately or after some specified delay
 *
 * classes can subscribe to certain event types and listen for the execution. All listeners to an event type are
 * notified upon the event executing and react based on their handler
 *
 */

#ifndef EVENTS_EVENT_CORE_H
#define EVENTS_EVENT_CORE_H

// std includes
#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
#include <utility>
#include <optional>
// project includes
#include "../../entities/hitbox.h"
#include "raylib.h"

// Forward declarations so registration events can carry pointers to live level
// entities without including the entity hierarchy (entities.h includes this
// header, so including it back would be a cycle). A pointer member needs only a
// forward declaration; the full type is required solely in the .cpp files that
// create the pointer (level) or dereference it (the cafe systems).
namespace entities{
	class table;
	class food_counter;
	class waiter_dog;
	class customer_dog;
	class dishwasher;
}

namespace events{
	enum customer_queue_side{
		left_queue = 0,
		right_queue = 1
	};
	// an enum ID for event types
	enum ids{
		move_frame = 1,
		right_mouse,
		left_mouse,
		move,
		remove,
		interact,
		menu_interact,
		select_dog,
		press_key,
		lvl_up,
		enter_edit,
		exit_edit,
		cursor_move,
		decoration_move,
		decoration_place,
		hold_edit,
		empty,
		register_table,
		register_customer,
		request_customer_table,
		customer_arrived,
		register_waiter,
		register_food_counter,
		waiter_arrived_table,
		order_served_id,
		send_waiter_table,
		customer_left,
		build_customer_dog_id,
		send_customer_position,
		debug_log_id,
		dog_path_complete,
		give_dog_path_id,
		table_removed,
		dog_to_station,
		food_counter_removed,
		dog_reached_station_id,
		waiter_removed,
		waiter_arrived_counter,
		table_cleared,
		register_dishwasher,
		dishwasher_removed,
		customer_removed,
		waiter_finished_clearing_id,
		waiter_collected_food_id,
		waiter_served_order_id,
		waiter_abandoned_serving_id,
		create,
		create_path_to_id,
		create_path_to_entity_id,
		animation_finished_id,
		size
	};
	class event{

		public:
			virtual ~event() = default;
			event(int id, float delay=0.0f)
			: handled_(false), type_(id), delay_(delay){}

			event(event&& other) = default;
			event& operator=(event&& other) = delete;

			bool is_handled(){
				return handled_;
			}
			int get_type() const{
					return type_;
			}
			bool update_delay(float delta){
				delay_ = std::max(0.0f, delay_ - delta);
				return delay_ <= 0.0f;
			}
			protected:
			bool handled_ = false;
			const int type_;
			float delay_; // potential execution delay for the event, i.e an event
			// can take palce 10s after another,
	};

	class empty_event : public event{
		public:
			empty_event()
			:event(ids::empty){}

			static int get_static_type(){
				return ids::empty;
			}
		private:
	};
	// when the player levels up, main listeners are shop items to check if the level requirement
	// is met to purhcase
	// and in future to play level up hud animations
	class level_up: public event{
		public:

			level_up(int new_level)
			:event(ids::lvl_up), new_level_(new_level){}

			static int get_static_type(){
				return ids::lvl_up;
			}
			int get_new_level() const {
				return new_level_;
			}
		private:
			const int new_level_;
	};
	class event_handler_interface{
		public:
		virtual ~event_handler_interface() = default;
		event_handler_interface() = default;
		event_handler_interface(const event_handler_interface& other) = default;
		event_handler_interface(event_handler_interface&& other) = default;

		event_handler_interface& operator=(const event_handler_interface& other) = delete;
		event_handler_interface& operator=(event_handler_interface&& other) = delete;
		void execute(const event& e){
			call_event(e);
		}
		virtual int get_type() const = 0;
		// identity, not type - unsubscribing one handler must not take out every
		// other listener for the same event
		virtual size_t get_handler_id() const = 0;
		private:
		virtual void call_event(const event& e) = 0;

	};

	template<typename E> // E for event
	class event_handler : public event_handler_interface{
	public:
		~event_handler() override = default;
		event_handler(std::function<void(const E& e)> handle)
			: handler_(handle), handler_type_(E::get_static_type()), handler_id_(next_handler_id_++){
			}

		event_handler(const event_handler& other) = default;
		event_handler(event_handler&& other) = default;

		event_handler& operator=(const event_handler& other) = delete;
		event_handler& operator=(event_handler&& other) = default;

		void call_event(const event& e) override{
			// check if event and handler template match, because you're doing a static
			// cast
			if(e.get_type() == E::get_static_type()){
				// trigger the event stored in the handler
				handler_(static_cast<const E&>(e));
			}
			return;
		}
		int get_type() const override{
			return handler_type_;
		}
		// copies keep the id - subscribe() stores a copy, and unsubscribe has to
		// be able to find that copy from the original
		size_t get_handler_id() const override{
			return handler_id_;
		}
		bool operator==(const event_handler& other){
			return handler_type_ == other.handler_type_;
		}
	private:
		inline static size_t next_handler_id_ = 0;

		std::function<void(const E& e)> handler_;
		const int handler_type_; // this should be an id i think
		size_t handler_id_;
	};
	// the event
	// the event handler - tempalted for any event, based on an interface
	// the dispatcher manages all the listeners for that
	class event_dispatcher {
	public:
		void subscribe(int event_key, std::unique_ptr<event_handler_interface>& handler_value);
		void unsubscribe(int event_key, size_t handler_id);
		void execute_event(const event& event);
		void queue_event(std::unique_ptr<event>& event);
		void add_delayed_event(std::unique_ptr<event>& event);
		void process_events(float delta);
	private:
		// for storing and processing events
		std::queue<std::unique_ptr<event>> event_queue_;
		std::vector<std::unique_ptr<event>> delayed_events_;
		// pairs an event id with instances of event handlers listening for the event
		std::unordered_map<int, std::vector<std::unique_ptr<event_handler_interface>>> subscriber_map_;
	};
	extern event_dispatcher global_dispatcher_;
}

#endif
