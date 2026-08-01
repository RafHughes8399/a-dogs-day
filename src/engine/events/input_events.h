/** Player input events: mouse, keyboard, cursor, camera, and
 * entity/menu-interaction hit-testing.
 */
#ifndef EVENTS_INPUT_EVENTS_H
#define EVENTS_INPUT_EVENTS_H

#include "event_core.h"

namespace events{
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
}

#endif
