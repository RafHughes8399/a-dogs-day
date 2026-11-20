/** Player input events: keyboard and menu-interaction hit-testing. */
#ifndef EVENTS_INPUT_EVENTS_H
#define EVENTS_INPUT_EVENTS_H

#include "event_core.h"
#include "hitbox.h"

namespace events{
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

	class cursor_created : public event{
		public:
			cursor_created(size_t id)
			:event(ids::cursor_created_id), id_(id){}

			static int get_static_type(){
				return ids::cursor_created_id;
			}
			size_t get_id() const{
				return id_;
			}
		private:
			const size_t id_;
	};

	class cursor_moved : public event{
		public:
			cursor_moved(size_t id, Vector2 new_position)
			:event(ids::cursor_moved_id), id_(id), new_position_(new_position){}
				size_t get_id() const{
					return id_;
				}
			
			static int get_static_type(){
				return ids::cursor_moved_id;
			}
			Vector2 get_postiion() const{
				return new_position_;
			}
		private:
			const size_t id_;
			const Vector2 new_position_;
	};

}

#endif
