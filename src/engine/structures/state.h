/**
 *  a state is what an entity is doing right now: the animation it holds while
 *  it does it, the work it does each frame, and the setup and teardown either
 *  side of it. states live in a state_machine, which owns the transitions
 *  between them.
 *
 *  author: raffa, september 2026
 */
#ifndef STATE_H
#define STATE_H

#include <cstddef>
#include <memory>
#include <optional>

namespace state{
    class state{
        public:
            virtual ~state() = default;
            state(size_t state_id, size_t animation)
            : state_id_(state_id), animation_(animation){}
            state(const state& other) = delete;
            state(state&& other) = delete;

            state& operator=(const state& other) = delete;
            state& operator=(state&& other) = delete;

            size_t get_state_id() const;
            size_t get_animation() const;

            virtual void update(size_t entity, float delta) = 0;
            virtual void on_transitioned_to(size_t entity, std::optional<size_t> payload);
            virtual void transitioning_to(size_t entity);
        private:
            const size_t state_id_;
            const size_t animation_;
    };

    class idle_state : public state{
        public:
            ~idle_state() override = default;
            idle_state(size_t state_id, size_t animation)
            : state(state_id, animation){}

            void update(size_t entity, float delta) override;
    };

    class stationary_state : public state{
        public:
            ~stationary_state() override = default;
            stationary_state(size_t state_id, size_t animation)
            : state(state_id, animation){}

            void update(size_t entity, float delta) override;
    };

    class walking_state : public state{
        public:
            ~walking_state() override = default;
            walking_state(size_t state_id, size_t animation)
            : state(state_id, animation){}

            void update(size_t entity, float delta) override;
    };

    class sitting_state : public state{
        public:
            ~sitting_state() override = default;
            sitting_state(size_t state_id, size_t animation)
            : state(state_id, animation){}

            void update(size_t entity, float delta) override;
    };

    class interacting_state : public state{
        public:
            ~interacting_state() override = default;
            interacting_state(size_t state_id, size_t animation)
            : state(state_id, animation){}

            void update(size_t entity, float delta) override;
    };

    class carrying_state : public state{
        public:
            ~carrying_state() override = default;
            carrying_state(size_t state_id, size_t animation)
            : state(state_id, animation), carried_item_(std::nullopt){}

            void update(size_t entity, float delta) override;
            void on_transitioned_to(size_t entity, std::optional<size_t> payload) override;
            void transitioning_to(size_t entity) override;

            std::optional<size_t> get_carried_item() const;
        private:
            std::optional<size_t> carried_item_;
    };

    class eating_state : public state{
        public:
            ~eating_state() override = default;
            eating_state(size_t state_id, size_t animation)
            : state(state_id, animation), elapsed_(0){}

            void update(size_t entity, float delta) override;
            void on_transitioned_to(size_t entity, std::optional<size_t> payload) override;

            int get_elapsed() const;
        private:
            int elapsed_;
    };
}
namespace state_builders{
    std::unique_ptr<state::state> build_player_idle_state();
    std::unique_ptr<state::state> build_player_walking_state();
    std::unique_ptr<state::state> build_player_interacting_state();

    std::unique_ptr<state::state> build_customer_walking_state();
    std::unique_ptr<state::state> build_customer_sitting_state();
    std::unique_ptr<state::state> build_customer_eating_state();

    std::unique_ptr<state::state> build_waiter_stationary_state();
    std::unique_ptr<state::state> build_waiter_idle_state();
    std::unique_ptr<state::state> build_waiter_interacting_state();
    std::unique_ptr<state::state> build_waiter_carrying_state();
}
#endif
