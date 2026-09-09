/**
 *  a state machine is a graph of states: the node at an index holds the state
 *  whose id is that index, and its edges name the transitions out of it. an
 *  edge is an event id, so a fact the game already raises is what moves an
 *  entity from one state to the next.
 *
 *  author: raffa, september 2026
 */
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "state.h"

namespace state_machine{
    class edge{
        public:
            ~edge() = default;
            edge(int transition, size_t destination)
            : transition_(transition), destination_(destination){}
            edge(const edge& other) = default;
            edge(edge&& other) = default;

            edge& operator=(const edge& other) = default;
            edge& operator=(edge&& other) = default;

            int get_transition() const;
            size_t get_destination() const;
        private:
            int transition_;
            size_t destination_;
    };

    class node{
        public:
            ~node() = default;
            node(std::unique_ptr<state::state> node_state, std::vector<edge> edges)
            : state_(std::move(node_state)), edges_(std::move(edges)){}
            node(const node& other) = delete;
            node(node&& other) = default;

            node& operator=(const node& other) = delete;
            node& operator=(node&& other) = default;

            state::state& get_state();
            const std::vector<edge>& get_edges() const;
        private:
            std::unique_ptr<state::state> state_;
            std::vector<edge> edges_;
    };

    using graph = std::vector<node>;

    class state_machine{
        public:
            ~state_machine() = default;
            state_machine()
            : graph_(), current_(0){}
            state_machine(graph machine_graph, size_t current)
            : graph_(std::move(machine_graph)), current_(current){}
            state_machine(const state_machine& other) = delete;
            state_machine(state_machine&& other) = default;

            state_machine& operator=(const state_machine& other) = delete;
            state_machine& operator=(state_machine&& other) = default;

            size_t current() const;
            size_t num_states() const;
            state::state* get_current_state();
            bool can_transition(int transition) const;
            void transition(size_t entity, int transition,
                std::optional<size_t> payload = std::nullopt);
            void update(size_t entity, float delta);
        private:
            std::optional<size_t> destination_of(int transition) const;

            graph graph_;
            size_t current_;
    };
}
namespace state_machine_builders{
    state_machine::state_machine build_player_state_machine();
    state_machine::state_machine build_customer_state_machine();
    state_machine::state_machine build_waiter_state_machine();
}
#endif
