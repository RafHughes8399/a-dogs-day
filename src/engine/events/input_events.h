/** Player input events: keyboard and menu-interaction hit-testing. */
#ifndef EVENTS_INPUT_EVENTS_H
#define EVENTS_INPUT_EVENTS_H

#include "event_core.h"

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
}

#endif
