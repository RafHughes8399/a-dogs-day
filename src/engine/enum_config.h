#ifndef ENUM_CONFIG_H
#define ENUM_CONFIG_H

#include "events/event_core.h"

namespace game_config{
    // * keyboard and mouse actions are separate enums, and `control`/`mouse_input`
    // * are separate structs, so a keyboard binding cannot carry a mouse action
    // * (or vice versa). While both lived in one enum, {KEY_F, mouse_press} was
    // * a legal value that nothing would catch.
    enum control_action{
        key_press = 0,
        key_hold,
        key_release
    };
    enum mouse_action{
        mouse_press = 3,
        mouse_hold,
        mouse_released,
        mouse_up
    };
}
namespace level_config{
    enum draw_layers{
        background = 0,
        decoration,
        stations,
        dogs,
        hud,
        cursor,
        size
    };

    // these indices are shared with the direction-ordered sprite arrays
    // (dog::set_direction_index indexes body_/head_ with a directions value), so
    // an entry here without a matching sprite is a trap for whoever adds an
    // animation next.
    // TODO (25 / 8 / 26) drop 'all' - it was an attempt at an omnidirectional cursor
    // TODO direction, but {1,1} is not a unit vector so it moves ~1.41x too fast
    // TODO when multiplied by move_speed, it has no sprite, and level_graph::
    // TODO position_to_node snaps it identically to 'right'. an entity with no
    // TODO facing wants no direction at all - i.e. no movement_component. see
    // TODO the note above ecs_entities::build_cursor.
    enum directions{
        left = 0,
        right,
        up,
        down,
        all,
        directions_size
    };
}
namespace animation_config{
    // * sheet rows, one enum per dog sprite slot. indices 0 - shared_size are the
    // * same animation on every part, so a whole-body action plays with one index
    // * across all four slots; part-specific rows start at shared_size.
    // * index 0 is what a freshly built sprite shows - a sprite's active animation
    // * starts there - so it is the resting row on every part.
    enum shared{
        idle = 0,
        downward_dog,
        eating,
        walking,
        sitting,
        interacting,
        carrying,
        shared_size
    };
    namespace tail{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            wag = shared::shared_size,
            size
        };
    }
    namespace body{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            pawing = shared::shared_size,
            size
        };
    }
    namespace head{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            pinned_back = shared::shared_size,
            one_up,
            bouncing,
            size
        };
    }
    namespace face{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            sniffing = shared::shared_size,
            panting,
            licking,
            size
        };
    }
    namespace station{
        enum tags{
            idle = 0,
            active,
            size
        };
    }
}
namespace controls_config{
    enum key_press_actions{
        dog_switch = KEY_F,
        shop_open = KEY_S,
        inventory_open = KEY_I,
        menu_open = KEY_TAB,
        quests_open = KEY_Q,
        map_open = KEY_M,
        back = KEY_ESCAPE,
        exit_edit = KEY_E,
        debug_toggle = KEY_J
    };
    enum key_hold_actions{
        edit_mode = KEY_E,
        move_down = KEY_DOWN,
        move_up  = KEY_UP,
        move_left = KEY_LEFT,
        move_right = KEY_RIGHT
    };
}
namespace entity_config{
    enum selectable_kinds{
        player_dog_kind = 0,
        decoration_kind,
        station_kind,
        customer_dog_kind,
        waiter_dog_kind,
        selectable_kinds_size
    };

    enum customers{
        tex = 0,
        customers_size
    };
    enum special_customers{
        garfield = customers_size,
        cumulative_customers_size
    };
    enum waiters{
        gianluca = 0,
        lionel,
        waiters_size
    };
    enum tables{
        dining_table = 0,
        tiled_table,
        test_table,
        tiled_table_orange,
        tiled_table_red,
        tiled_table_green,
        tiled_table_blue,
        tables_size
    };
    enum counters{
        food_counter = 0,
        counters_size
    };
    enum cooking_stations{
        stove = 0,
        oven,
        espresso_station,
        mini_fridge,
        bush,
        chopping_board,
        cooking_stations_size
    };
    enum foods{
        lasagna = 0,
        bone_biscuits,

        espresso,
        pup_cup,

        cutlets,
        boiled_chicken,
        risotto,
        carbonara,

        bird,
        sausage_on_a_stick,
        pork_knuckles,
        lemongrass,
        mutton,
        muttoff,
        prozac,
        charpoocherie_board,
        cheddar,
        parmigiano,
        shishamo,
        rabbit_foot,

        brisket_leftovers,
        steak_leftovers,
        escargot,
        foods_size
    };
    enum counter_sprite_slots{
        counter_body = 0,
        counter_food,
        counter_sprite_slots_size
    };
    enum dog_sprite_slots{
        dog_body = 0,
        dog_tail,
        dog_head,
        dog_face,
        dog_sprite_slots_size
    };
    enum dog_part_directions{
        dog_part_left = 0,
        dog_part_right,
        dog_part_directions_size
    };
    // sprite attributes, stored as an array of four numbers [frame width, frame height, frames, animations]
    enum attributes{
        frame_width = 0,
        frame_height,
        frames,
        animations,
        size
    };
}
namespace hud_config{
    enum click_actions{
        select_recipe = 0,
        change_page,
        move_decoration,
        sell_decoration,
        store_decoration,
        size
    };
}
namespace interaction_config{
    enum interactions{
        customer_table_sit = 0,
        waiter_table_serve,
        waiter_counter_pickup,
        waiter_counter_place_down,
        player_station_cook,
        size
    };
}
namespace dog_config{
    enum player_dog_states{
        player_idle = 0,
        player_walking,
        player_interacting,
        player_states_size
    };
    enum customer_dog_states{
        customer_walking = 0,
        customer_sitting,
        customer_eating,
        customer_states_size
    };
    enum waiter_dog_states{
        waiter_stationary = 0,
        waiter_idle,
        waiter_carrying,
        waiter_states_size
    };
    enum state_transitions{
        path_created = events::ids::dog_started_path_id,
        path_finished = events::ids::dog_path_complete,
        player_work_station = events::ids::player_work_station_id,
        interaction_finished = events::ids::interaction_finished_id,
        order_served = events::ids::order_served_id,
        food_dropped = events::ids::food_dropped_id,
        food_collected = events::ids::waiter_collected_food_id,
        meal_finished = events::ids::customer_finished_meal_id,
        customer_leaving = events::ids::customer_left
    };
}
namespace state_machine_config{
    enum cook_station_states{
        station_idle = 0,
        station_active,
        station_states_size
    };
    enum state_transitions{
        player_engaged = events::ids::player_engaged_id,
        player_disengaged = events::ids::player_disengaged_id
    };
}
#endif
