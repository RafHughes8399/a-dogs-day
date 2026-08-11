#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

#include "events.h"
#include "raylib.h"

#include <deque>
#include <memory>
#include <string>

namespace debug{
    class logger{
        public:
            class state{
                public:
                    virtual ~state() = default;
                    state() = default;
                    state(const state& other) = default;
                    state(state&& other) = default;

                    state& operator=(const state& other) = default;
                    state& operator=(state&& other) = default;

                    virtual void render(logger& logger) = 0;
            };

            class inactive : public state{
                public:
                    void render(logger& logger) override;
            };

            class active : public state{
                public:
                    void render(logger& logger) override;
            };

            static logger& get_instance();

            logger(const logger& other) = delete;
            logger(logger&& other) = delete;

            logger& operator=(const logger& other) = delete;
            logger& operator=(logger&& other) = delete;

            void update(float delta);
            void render();
            void toggle();
            void toggle_pause();
            void set_frame(int frame);
            void on_debug_log_event(const events::debug_log& event);

        private:
            logger();
            ~logger() = default;

            void subscribe();
            void unsubscribe();
            void add_message(const std::string& message);
            std::string timestamp();
            void render_backdrop();
            void render_messages();

            std::unique_ptr<state> state_;
            events::event_handler<events::debug_log> debug_log_handler_;
            // a std::queue would model the push_back / pop_front use exactly but
            // cannot be iterated, and every message is drawn every frame
            std::deque<std::string> messages_;
            int frame_;
            bool subscribed_;
            bool paused_;
    };
}

#endif
