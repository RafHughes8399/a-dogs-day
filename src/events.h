
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
		seat_customer_at_table = 21,
		customer_arrived = 22,
		customer_sent_to_table = 23,
		size = 24
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

	class test_event : public  event{
	public:

		test_event(float delay=0.0f)
		: event(ids::test, delay), time_(std::time(nullptr)){}

		static int get_static_type(){
			return ids::test;
		}
		char* get_event_time() const{
			return std::asctime(std::localtime(&time_));
		}
		private:
		const std::time_t time_;

	};
	// for when the player finishes holding down the edit button to switch between edit and non-edit mode, the cursor
	// listens to change its state
	class enter_edit_mode : public event{
		public:
			enter_edit_mode()
			:event(ids::enter_edit){}

			static int get_static_type(){
				return ids::enter_edit;
			}
		private:
	};
	class exit_edit_mode : public event{
		public:
			exit_edit_mode()
			:event(ids::exit_edit){}

			static int get_static_type(){
				return ids::exit_edit;
			}
		private:
	};
	// for when the player holds down the edit key, main listener is the edit wheel hud component
	class edit_hold : public event{
		public:
			edit_hold(Vector2 position, int frame)
			:event(ids::hold_edit), position_(position), edit_progress_(frame){}

			static int get_static_type(){
				return ids::hold_edit;
			}
			Vector2 get_position() const{
				return position_;
			}
			int get_edit_progress() const{
				return edit_progress_;
			}
		private:
			const Vector2 position_;
			const int edit_progress_;

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
	// for when an entity potentailly interacts with another, main listener is the quad tree to check collisionss
	class interact_entity : public event{
		public:

			interact_entity(size_t id, hitbox::hitbox hitbox)
			:event(ids::interact), id_(id), hitbox_(hitbox){}

			static int get_static_type(){
				return ids::interact;
			}
			size_t get_id() const {
				return id_;
			}
			const hitbox::hitbox& get_hitbox() const{
				return hitbox_;
			}
			private:
			const size_t id_;
			const hitbox::hitbox hitbox_; 
		};

		// for when the cursor potentially interacts with a button on a menu, main listener is the meny graph that checks the buttons
		// on the current menu 
	class interact_menu : public event {
		public:
			interact_menu(hitbox::hitbox hitbox)
			:event(ids::menu_interact), hitbox_(hitbox){}

			static int get_static_type(){
				return ids::menu_interact;
			}
			const hitbox::hitbox& get_hitbox() const{
				return hitbox_;
			}
		private:
			const hitbox::hitbox hitbox_; 
	};
		// when a key is pressed by the player, main listeners are the menus for menu navigation

	class key_press: public event{
		public:	

			key_press(int key)
			:event(ids::press_key), key_(key){}

			static int get_static_type(){
				return ids::press_key;
			}
			int get_key() const {
				return key_;
			}
		private:
			const int key_;
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
	// when the cursor left click occurs, main listener is the quad tree to check collisions
	class left_mouse_click : public event{
		public:
			left_mouse_click(Vector2 position, float box_width, float box_length)
			:	event(ids::left_mouse), mouse_position_(position), collision_box_(Rectangle{position.x, position.y, box_width, box_length}){ // TODO fill in (16/12)
			}
			static int get_static_type(){
				return ids::left_mouse;
			}
			Vector2 get_mouse_position() const{
				return mouse_position_;
			}
			Rectangle get_hitbox() const{
				return collision_box_;
			}
			
		private:
			const Vector2 mouse_position_;
			const Rectangle collision_box_;
	};

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

	class moved_cursor : public event{
		public:
			moved_cursor(Vector2 position)
			: event(ids::cursor_move), new_position_(position){}

			static int get_static_type(){
				return ids::cursor_move;
			}
			
			Vector2 get_position() const{
				return new_position_;
			}
		private:
			const Vector2 new_position_;
	};
	class moved_decoration : public event{
		public:
			moved_decoration(Rectangle pre, Rectangle post, int id)
			: event(ids::decoration_move), pre_move_(pre), post_move_(post), id_(id){}

			static int get_static_type(){
				return ids::decoration_move;
			}
			Rectangle get_pre_move() const{
				return pre_move_;
			}
			Rectangle get_post_move() const {
				return post_move_;
			} 
			int get_id() const {
				return id_;
			}
		private:
			const Rectangle pre_move_;
			const Rectangle post_move_;
			const int id_;
	};
	class placed_decoration : public event{
		public:
			placed_decoration(Rectangle rec, size_t id)
			: event(ids::decoration_place), rectangle_(rec), id_(id){}

			static int get_static_type(){
				return ids::decoration_place;
			}
			Rectangle get_rectangle() const{
				return rectangle_;
			}

			size_t get_id() const {
				return id_;
			}
		private:
			const Rectangle rectangle_;
			const size_t id_;
	};
	// Cafe-domain fact: a table entity exists in the level and can be tracked by
	// the maitre d'. The event carries ids only so the cafe/order system does not
	// need concrete entity types or ownership of level entities.
	class registered_table : public event{
		public:
			registered_table(size_t table_id)
			: event(ids::register_table), table_id_(table_id){}

			static int get_static_type(){
				return ids::register_table;
			}
			size_t get_table_id() const{
				return table_id_;
			}
		private:
			const size_t table_id_;
	};
	// Cafe-domain fact: a customer dog exists in the level and can enter the
	// restaurant flow. The maitre d' records the id, not the dog object.
	class registered_customer : public event{
		public:
			registered_customer(size_t customer_id)
			: event(ids::register_customer), customer_id_(customer_id){}

			static int get_static_type(){
				return ids::register_customer;
			}
			size_t get_customer_id() const{
				return customer_id_;
			}
		private:
			const size_t customer_id_;
	};
	// Cafe-domain fact/request: a customer dog needs a table assignment. The
	// maitre d' listens for this and resolves it into command events later.
	class requested_customer_table : public event{
		public:
			requested_customer_table(size_t customer_id)
			: event(ids::request_customer_table), customer_id_(customer_id){}

			static int get_static_type(){
				return ids::request_customer_table;
			}
			size_t get_customer_id() const{
				return customer_id_;
			}
		private:
			const size_t customer_id_;
	};
	// Cafe-domain command: the maitre d' has decided which customer should sit at
	// which table. A world-owning system, such as the level, should listen for
	// this event and perform the concrete entity mutation/pathing by id.
	class seat_customer_at_table : public event{
		public:
			seat_customer_at_table(size_t customer_id, size_t table_id)
			: event(ids::seat_customer_at_table), customer_id_(customer_id), table_id_(table_id){}

			static int get_static_type(){
				return ids::seat_customer_at_table;
			}
			size_t get_customer_id() const{
				return customer_id_;
			}
			size_t get_table_id() const{
				return table_id_;
			}
		private:
			const size_t customer_id_;
			const size_t table_id_;
	};
	// Cafe-domain fact: a customer dog has entered the cafe and should be placed
	// into the physical waiting queue managed by the maitre d'.
	class customer_dog_arrived : public event{
		public:
			customer_dog_arrived(size_t customer_id)
			: event(ids::customer_arrived), customer_id_(customer_id){}

			static int get_static_type(){
				return ids::customer_arrived;
			}
			size_t get_customer_id() const{
				return customer_id_;
			}
		private:
			const size_t customer_id_;
	};
	// Cafe-domain command/fact: the maitre d' has taken a customer dog out of
	// the waiting queue and sent it toward an assigned table.
	class customer_dog_sent_to_table : public event{
		public:
			customer_dog_sent_to_table(size_t customer_id, size_t table_id)
			: event(ids::customer_sent_to_table), customer_id_(customer_id), table_id_(table_id){}

			static int get_static_type(){
				return ids::customer_sent_to_table;
			}
			size_t get_customer_id() const{
				return customer_id_;
			}
			size_t get_table_id() const{
				return table_id_;
			}
		private:
			const size_t customer_id_;
			const size_t table_id_;
	};
	// when an view_Frame moves, main listener is the level to adjust the view_frame when the player
	// moves it
	class move_view_frame : public event{
		public:

		move_view_frame(Vector2 delta)
		: event(ids::move_frame), delta_(delta){}
		
		static int get_static_type(){
			return ids::move_frame;
		}
		Vector2 get_delta() const{
			return delta_;
		}
		private:
		const Vector2 delta_;
	};
	// when a right click occurs, main listener is the level to create a paw mark
	class right_mouse_click : public event{
		public:
			right_mouse_click(Vector2 position, int selected_dog)
			: event(ids::right_mouse), mouse_position_(position), selected_dog_(selected_dog){}

			static int get_static_type(){
				return ids::right_mouse;
			}
			Vector2 get_mouse_position() const{
				return mouse_position_;
			}
			int get_selected_dog() const {
				return selected_dog_;
			}
		private:
			const Vector2 mouse_position_;
			const int selected_dog_;
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

	// for when a dog is selected, main listener is the player to update the id
	class selected_dog : public event{
		public:
			selected_dog(size_t id)
			:event(ids::select_dog), id_(id){}

			static int get_static_type(){
				return ids::select_dog;
			}
			size_t get_id() const {
				return id_;
			}
		private:
			const size_t id_;
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
