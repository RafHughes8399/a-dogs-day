/** Debug/logging events. */
#ifndef EVENTS_DEBUG_EVENTS_H
#define EVENTS_DEBUG_EVENTS_H

#include "event_core.h"

namespace events{
	class debug_log : public event{
		public:
			debug_log(std::string message)
			:event(ids::debug_log_id), message_(std::move(message)){}

			static int get_static_type(){
				return ids::debug_log_id;
			}
			const std::string& get_message() const{
				return message_;
			}
		private:
			const std::string message_;
	};
}

#endif
