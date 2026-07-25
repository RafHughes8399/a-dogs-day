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
		right_mouse = 2,
		left_mouse = 3,
		move = 4,
		remove = 5,
		interact = 6,
		menu_interact = 7,
		select_dog = 8,
		press_key = 9,
		lvl_up = 10,
		enter_edit = 11,
		exit_edit = 12,
		cursor_move = 13,
		decoration_move = 14,
		decoration_place = 15,
		hold_edit = 16,
		empty = 17,
		register_table = 18,
		register_customer = 19,
		request_customer_table = 20,
		customer_arrived = 22,
		register_waiter = 24,
		register_food_counter = 25,
		waiter_arrived_table = 27,
		order_served_id = 28,
		send_waiter_table = 31,
		customer_left = 33,
		build_customer_dog_id = 34,
		send_customer_position = 35,
		debug_log_id = 36,
		dog_path_complete = 37,
		give_dog_path_id = 38,
		table_removed = 39,
		dog_to_station = 40,
		food_counter_removed = 41,
		dog_reached_station_id = 42,
		waiter_removed = 44,
		waiter_arrived_counter = 45,
		table_cleared = 46,
		register_dishwasher = 47,
		dishwasher_removed = 48,
		customer_removed = 49,
		waiter_finished_clearing_id = 50,
		waiter_collected_food_id = 51,
		waiter_served_order_id = 52,
		waiter_abandoned_serving_id = 53,
		size = 54
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
		private:
		virtual void call_event(const event& e) = 0;

	};

	template<typename E> // E for event
	class event_handler : public event_handler_interface{
	public:
		~event_handler() override = default;
		event_handler(std::function<void(const E& e)> handle)
			: handler_(handle), handler_type_(E::get_static_type()){
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
		bool operator==(const event_handler& other){
			return handler_type_ == other.handler_type_;
		}
	private:
		std::function<void(const E& e)> handler_;
		const int handler_type_; // this should be an id i think
	};
	// the event
	// the event handler - tempalted for any event, based on an interface
	// the dispatcher manages all the listeners for that
	class event_dispatcher {
	public:
		void subscribe(int event_key, std::unique_ptr<event_handler_interface>& handler_value);
		void unsubscribe(int event_key, const int handler_value);
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
