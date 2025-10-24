
/** the core of the event system, defines event types, classes for handling events and the primary dispatcher 
 * that is responsible for queueing and executing events 
 * 
 * takes advantage of a obsever-listener pattern, enables classes and components within the codebase to 
 * queue events with certain information upon something occuring (like an object moving) that are then executed either 
 * immediately or after some specified delay
 * 
 * classes can subscribe to certain event types and listen for the execution. All listeners to an event type are 
 * notified upon the event executing and react based on their handler
 * 
 */

#ifndef EVENTS_H
#define EVENTS_H

// std includes 
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>

// project includes
#include "raylib.h"

namespace events{
	// an enum ID for event types
	enum ids{
		test = 0,
		left_mouse = 1,
		right_mouse = 2,
		size = 3
	};
	class event{
	protected:
		bool handled_ = false;
		const int type_;
		float delay_; // potential execution delay for the event, i.e an event 
		// can take palce 10s after another,
	public:
		virtual ~event() = default;	
		event(int id, float delay=0.0f)
		: type_(id), handled_(false), delay_(delay){};
		
		event(event&& other) = default;
		event& operator=(event&& other) = default;
		
		bool is_handled(){
			return handled_;
		}
		const int get_type() const{
				return type_;
		}
		bool update_delay(float delta){
			delay_ = std::max(0.0f, delay_ - delta);
			return delay_ == 0;
		}
	};

	class test_event : public  event{
	public:
		~test_event() = default;
		test_event(float delay=0.0f)
		: event(ids::test, delay), time_(std::time(nullptr)){};

		static const int get_static_type(){
			return ids::test;
		}
		char* get_event_time() const{
			return std::asctime(std::localtime(&time_));
		}
		private:
		std::time_t time_;

	};
	// ------------------------- mouse events  ------------------------- //
	class left_mouse_down : public event{
		public:
			~left_mouse_down() = default;
			left_mouse_down(Vector2 delta)
			: event(ids::left_mouse), mouse_delta_(delta){};

			static const int get_static_type(){
				return ids::left_mouse;
			}
			Vector2 get_mouse_delta() const{
				return mouse_delta_;
			}
		private:
			Vector2 mouse_delta_;
	};
	class right_mouse_click : public event{
		public:
			~right_mouse_click() = default;
			right_mouse_click(Vector2 position)
			: event(ids::right_mouse), mouse_position_(position){};

			static const int get_static_type(){
				return ids::right_mouse;
			}
			Vector2 get_mouse_position() const{
				return mouse_position_;
			}
		private:
			Vector2 mouse_position_;
	};

	// ------------------------- entity events  ------------------------- //
	// ! right now for removing the paw when it fades ! 
	// does it need to be an event though, because the tree can handle entity removing, 
	// just pass it through an update status
	class remove_entity : public event{

	};
	class event_handler_interface{
		public:
		virtual ~event_handler_interface() = default;
		void execute(const event& e){
			call_event(e);
		}
		virtual const int get_type() const = 0;
		private:
		virtual void call_event(const event& e) = 0;

	};
	
	template<typename E> // E for event
	class event_handler : public event_handler_interface{
	public:
		~event_handler() override = default;
		event_handler(std::function<void(const E& e)> handle)
			: handler_type_(E::get_static_type()), handler_(handle){
			};
		
		event_handler(const event_handler& other) = default;
		event_handler(event_handler&& other) = default;
		
		event_handler& operator=(const event_handler& other) = default;
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
		const int get_type() const override{
			return handler_type_;
		}
		bool operator==(const event_handler& other){
			return handler_type_ == other.handler_type_; 
		}
	private:
		std::function<void(const E& e)> handler_;
		const int handler_type_;
	};

	
	// 
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