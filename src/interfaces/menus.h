#ifndef MENU_H
#define MENU_H

#include <optional>
#include <utility>
#include <vector>

#include "config.h"
namespace menus{
    enum menu_ids{
        blank = 0,
        pause = 1,
        tab = 2,
        inventory = 3,
        map = 4,
        shop = 5,
        quest = 6,
        menu_ids_size
    };
    class menu_graph{
        public:
            class node{
                public:
                    ~node() = default;
                    node(size_t id)
                    : id_(id), roots_(), active_(0){}
                    node(const node& other) = default;
                    node(node&& other) = default;
                    node& operator=(const node& other) = default;
                    node& operator=(node&& other) = default;

                    size_t get_id() const;
                    const std::vector<size_t>& get_roots() const;
                    std::optional<size_t> get_active_root() const;
                    void add_root(size_t root);
                    void next_root();
                    void previous_root();
                private:
                    size_t id_;
                    std::vector<size_t> roots_;
                    size_t active_;
            };
            class edge{
                public:
                    ~edge() = default;
                    edge(int key, size_t destination)
                    : key_(key), destination_(destination){}
                    edge(const edge& other) = default;
                    edge(edge&& other) = default;
                    edge& operator=(const edge& other) = default;
                    edge& operator=(edge&& other) = default;

                    int get_key() const;
                    size_t get_destination() const;
                private:
                    int key_;
                    size_t destination_;
            };

            ~menu_graph() = default;
            menu_graph()
            : current_(menu_ids::blank), graph_(){
                build_graph();
            }
            menu_graph(const menu_graph& other) = default;
            menu_graph(menu_graph&& other) = default;
            menu_graph& operator=(const menu_graph& other) = delete;
            menu_graph& operator=(menu_graph&& other) = delete;

            size_t navigate(int key);
            void add_root(size_t menu, size_t root);
            node& get_node(size_t menu);

            size_t current_;
        private:
            void build_graph();
            std::vector<std::pair<node, std::vector<edge>>> graph_;
    };
}
#endif
