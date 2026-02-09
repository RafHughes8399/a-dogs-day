
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
#include "hitbox.h"
#include "raylib.h"
namespace events{
	// an enum ID for event types
	enum ids{
		test = 0,
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
		edit_switch = 11,
		cursor_move = 12,
		decoration_move = 13,
		size = 14
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
	// for when the player finishes holding down the edit button to switch between edit and non-edit mode, the cursor
	// listens to change its state
	class edit_mode : public event{
		public:
			~edit_mode() = default;
			edit_mode()
			:event(ids::edit_switch){};

			static const int get_static_type(){
				return ids::edit_switch;
			}
		private:
	};
	// for when an entity potentailly interacts with another, main listener is the quad tree to check collisionss
	class interact_entity : public event{
		public:
			~interact_entity() = default;
			interact_entity(size_t id, hitbox::hitbox hitbox)
			:event(ids::interact), id_(id), hitbox_(hitbox){};

			static const int get_static_type(){
				return ids::interact;
			}
			size_t get_id() const {
				return id_;
			}
			const hitbox::hitbox& get_hitbox() const{
				return hitbox_;
			}
			private:
			size_t id_;
			hitbox::hitbox hitbox_; 
		};

		// for when the cursor potentially interacts with a button on a menu, main listener is the meny graph that checks the buttons
		// on the current menu 
	class interact_menu : public event {
		public:
			~interact_menu() = default;
			interact_menu(hitbox::hitbox hitbox)
			:event(ids::menu_interact), hitbox_(hitbox){};

			static const int get_static_type(){
				return ids::menu_interact;
			}
			const hitbox::hitbox& get_hitbox() const{
				return hitbox_;
			}
		private:
			hitbox::hitbox hitbox_; 
	};
		// when a key is pressed by the player, main listeners are the menus for menu navigation

	class key_press: public event{
		public:	
			~key_press() = default;
			key_press(int key)
			:event(ids::press_key), key_(key){};

			static const int get_static_type(){
				return ids::press_key;
			}
			int get_key() const {
				return key_;
			}
		private:
			int key_;
	};
	// when the player levels up, main listeners are shop items to check if the level requirement 
	// is met to purhcase 
	// and in future to play level up hud animations
	class level_up: public event{
		public:	
			~level_up() = default;
			level_up(int new_level)
			:event(ids::lvl_up), new_level_(new_level){};

			static const int get_static_type(){
				return ids::lvl_up;
			}
			int get_new_level() const {
				return new_level_;
			}
		private:
			int new_level_;
	};
	// when the cursor left click occurs, main listener is the quad tree to check collisions
	class left_mouse_click : public event{
		public:
			~left_mouse_click() = default;
			left_mouse_click(Vector2 position, float box_width, float box_length)
			:	event(ids::left_mouse), mouse_position_(position), collision_box_(Rectangle{position.x, position.y, box_width, box_length}){ // TODO fill in (16/12)

			};
			static const int get_static_type(){
				return ids::left_mouse;
			}
			Vector2 get_mouse_position() const{
				return mouse_position_;
			}
			Rectangle get_hitbox() const{
				return collision_box_;
			}
			
		private:
			Vector2 mouse_position_;
			Rectangle collision_box_;
	};

	// when an entity moves, main listener is the quad tree to move entities into the correct node
	class move_entity : public event{
		public:
		~move_entity() = default;
		move_entity(size_t id)
			: event(ids::move), id_(id){};
		static const int get_static_type(){
			return ids::move;
		}
		size_t get_id() const{
			return id_;
		}
		private:
		size_t id_;
	};

	class moved_cursor : public event{
		public:
			~moved_cursor() = default;
			moved_cursor(Vector2 position)
			: event(ids::cursor_move), new_position_(position){};

			static const int get_static_type(){
				return ids::cursor_move;
			}
			
			Vector2 get_position() const{
				return new_position_;
			}
		private:
			Vector2 new_position_;
	};
	class moved_decoration : public event{
		public:
			~moved_decoration() = default;
			moved_decoration(Rectangle pre, Rectangle post, size_t id)
			: event(ids::decoration_move), pre_move_(pre), post_move_(post), id_(id){}

			static const int get_static_type(){
				return ids::decoration_move;
			}
			Rectangle get_pre_move() const{
				return pre_move_;
			}
			Rectangle get_post_move() const {
				return post_move_;
			} 
			size_t get_id() const {
				return id_;
			}
		private:
			Rectangle pre_move_;
			Rectangle post_move_;
			size_t id_;
	};
	// when an view_Frame moves, main listener is the level to adjust the view_frame when the player
	// moves it
	class move_view_frame : public event{
		public:
		~move_view_frame() = default;
		move_view_frame(Vector2 delta)
		: event(ids::move_frame), delta_(delta){};
		
		static const int get_static_type(){
			return ids::move_frame;
		}
		Vector2 get_delta() const{
			return delta_;
		}
		private:
		Vector2 delta_;
	};
	// when a right click occurs, main listener is the level to create a paw mark
	class right_mouse_click : public event{
		public:
			~right_mouse_click() = default;
			right_mouse_click(Vector2 position, int selected_dog)
			: event(ids::right_mouse), mouse_position_(position), selected_dog_(selected_dog){};

			static const int get_static_type(){
				return ids::right_mouse;
			}
			Vector2 get_mouse_position() const{
				return mouse_position_;
			}
			int get_selected_dog() const {
				return selected_dog_;
			}
		private:
			Vector2 mouse_position_;
			int selected_dog_;
	};
	// for when an entity need be removed from the level, main listener is the quad tree
	class remove_entity : public event{
		public:
			~remove_entity() = default;
			remove_entity(size_t id)
			: event(ids::remove), id_(id){};

			static const int get_static_type(){
				return ids::remove;
			}
			size_t get_id() const{
				return id_;
			}
		private:
			size_t id_;

	};

	// for when a dog is selected, main listener is the player to update the id
	class selected_dog : public event{
		public:
			~selected_dog() = default;
			selected_dog(size_t id)
			:event(ids::select_dog), id_(id){};

			static const int get_static_type(){
				return ids::select_dog;
			}
			size_t get_id() const {
				return id_;
			}
		private:
			size_t id_;
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