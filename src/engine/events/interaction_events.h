/** facts raised by the interaction system as a handshake between an interactor
 * and an interactable opens and closes.
 *
 * queued, not executed - a started or finished interaction is a fact the next
 * frame acts on, and queueing lands it in process_events rather than part way
 * through interaction_system's own handling.
 */
#ifndef EVENTS_INTERACTION_EVENTS_H
#define EVENTS_INTERACTION_EVENTS_H

#include "event_core.h"

namespace events{
	class player_work_station : public event{
		public:
			player_work_station(size_t interactor_id, size_t interactee_id)
			: event(ids::player_work_station_id), interactor_id_(interactor_id),
			interactee_id_(interactee_id){}

			static int get_static_type(){
				return ids::player_work_station_id;
			}
			size_t get_interactor_id() const{
				return interactor_id_;
			}
			size_t get_interactee_id() const{
				return interactee_id_;
			}
		private:
			const size_t interactor_id_;
			const size_t interactee_id_;
	};

	class interaction_finished : public event{
		public:
			interaction_finished(size_t interactor_id, size_t interactee_id)
			: event(ids::interaction_finished_id), interactor_id_(interactor_id),
			interactee_id_(interactee_id){}

			static int get_static_type(){
				return ids::interaction_finished_id;
			}
			size_t get_interactor_id() const{
				return interactor_id_;
			}
			size_t get_interactee_id() const{
				return interactee_id_;
			}
		private:
			const size_t interactor_id_;
			const size_t interactee_id_;
	};
}
#endif
