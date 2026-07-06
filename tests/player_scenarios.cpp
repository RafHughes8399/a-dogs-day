// Scenarios for the player-controlled dogs (mack / khiri): construction,
// selection switching, and right-click movement.
//
// STUBS: each SCENARIO calls SKIP(...) so it reports as *skipped*. Intended
// flow is in comments; `(needs helper: X)` marks an action test_game does not
// expose yet.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"

using testing::test_game;

SCENARIO("a player dog is built and inserted into the level", "[player][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a fresh game
    //   test_game game;
    // WHEN mack is built and inserted on the dogs layer
    //   game.insert_entity(game.build_mack(level_config::mack_id, spawn_pos),
    //                      level_config::dogs);
    // THEN the level contains a dog with mack's id
    //   REQUIRE(game.find_entity(level_config::mack_id) != nullptr);
}

SCENARIO("the selected player dog can be switched", "[player][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN both mack and khiri inserted, mack selected by default
    //   test_game game;
    //   game.insert_entity(game.build_mack(level_config::mack_id, pos_a),  level_config::dogs);
    //   game.insert_entity(game.build_khiri(level_config::khiri_id, pos_b), level_config::dogs);
    // WHEN the selection is switched
    //   (needs helper: test_game::switch_selected_dog() / a player/controls hook)
    // THEN subsequent player commands target khiri, not mack
    //   (assert via which dog responds to a move command below)
}

SCENARIO("right-clicking sends the selected player dog to a position", "[player][controls][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a game with the selected player dog inserted
    //   test_game game;
    //   game.insert_entity(game.build_mack(level_config::mack_id, start_pos), level_config::dogs);
    // WHEN a right-click at a target position is issued
    //   (needs helper: test_game::right_click_at(target) -> fire events::right_mouse_click,
    //    the same path controls uses; no synthetic OS mouse input)
    //   game.tick_until([&]{ return !game.get_player_dog(mack_id).get_current_path().empty(); }, 60);
    // THEN the dog has a path toward that target
    //   (needs helper: get_player_dog / reuse get_current_path accessor; DOG_DAYS_TESTING)
}
