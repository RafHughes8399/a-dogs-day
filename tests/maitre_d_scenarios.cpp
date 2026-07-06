// Scenarios for the maitre d' domain: customer arrival + queueing, tables
// (build / insert / move), table assignment, and customer state transitions.
//
// STUBS: each SCENARIO calls SKIP(...) so it reports as *skipped*. Intended
// flow is in comments; `(needs helper: X)` marks an action test_game does not
// expose yet.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"

using testing::test_game;

SCENARIO("a customer dog arrives and paths to the queue head", "[maitre_d][customer][pathfinding][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a customer dog inserted at the entrance
    //   test_game game;
    //   game.insert_customer_dog(1, maitre_d::entrance_);
    // WHEN a debug arrival is fired and the sim advances
    //   game.customer_arrives();
    //   game.tick_until([&]{ return game.get_customer_dog(1).get_state_name()
    //                               != std::string("default_state"); }, 60);
    // THEN the dog has a real path toward its queue slot / the queue head
    //   REQUIRE_FALSE(game.get_customer_dog(1).get_current_path().empty());
    //   (needs accessors: get_state_name / get_current_path; DOG_DAYS_TESTING, spec step 5)
}

SCENARIO("a table is built and inserted into the level", "[maitre_d][table][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a fresh game
    //   test_game game;
    // WHEN a table is built and inserted on the stations layer
    //   (needs helper: test_game::build_table(id, pos) wrapping e_builder.build_table)
    //   game.insert_entity(game.build_table(10, table_pos), level_config::stations);
    // THEN the level holds the table AND the maitre d' has registered it
    //   REQUIRE(game.find_entity(10) != nullptr);
    //   (insertion should emit events::registered_table -> maitre_d records it;
    //    assert via a maitre_d table-count/lookup accessor - needs helper)
}

SCENARIO("a table can be moved", "[maitre_d][table][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a table inserted and registered
    //   test_game game;
    //   game.insert_entity(game.build_table(10, table_pos), level_config::stations);
    // WHEN the table is moved to a new position
    //   (needs helper: test_game::move_table(10, new_pos) / fire the move event)
    // THEN the maitre d' tracks the new position (status unchanged - level owns position)
}

SCENARIO("the maitre d' assigns a queued customer to a free table", "[maitre_d][queue][table][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a registered free table and a customer that has reached the queue head
    //   test_game game;
    //   game.insert_entity(game.build_table(10, table_pos), level_config::stations);
    //   game.insert_customer_dog(1, maitre_d::entrance_);
    //   game.customer_arrives();
    //   game.tick(delta, /*frames=*/ enough_to_reach_head);
    // WHEN the maitre d' processes assignments (inside tick: maitre_d_.update)
    //   game.tick_until([&]{ return game.get_customer_dog(1).get_state_name()
    //                               == std::string("walking_to_table"); }, 120);
    // THEN the customer is walking to the assigned table and the table is reserved
    //   REQUIRE_FALSE(game.get_customer_dog(1).get_current_path().empty());
    //   (table-reserved assertion needs a maitre_d accessor - helper)
}

SCENARIO("a customer dog transitions through its states", "[maitre_d][customer][stub]"){
    SKIP("stub - not yet implemented");
    // Walks the customer lifecycle: default_state -> walking_to_table -> seated
    // -> eating -> leaving, ticking and asserting get_state_name() at each step.
    // NOTE: per current work notes, only default_state and walking_to_table are
    // functional today; seated/eating/leaving are stubs/unreachable - so this
    // scenario is a placeholder until those states are built out.
    //   test_game game;
    //   game.insert_customer_dog(1, maitre_d::entrance_);
    //   game.customer_arrives();
    //   game.tick_until(... walking_to_table ...);
    //   REQUIRE(game.get_customer_dog(1).get_state_name() == std::string("walking_to_table"));
    //   ... (seated / eating / leaving once implemented)
}
