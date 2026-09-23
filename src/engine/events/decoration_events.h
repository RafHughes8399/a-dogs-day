/** Level-editor and decoration-placement events. */
#ifndef EVENTS_DECORATION_EVENTS_H
#define EVENTS_DECORATION_EVENTS_H

#include "event_core.h"

namespace events{
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
}

#endif
