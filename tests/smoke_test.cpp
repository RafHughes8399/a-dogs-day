#include <catch2/catch_test_macros.hpp>

// Minimal smoke test: proves the Catch2 build/link/run loop works before any
// real game-logic scenarios (test_game, dog state machines, etc.) are wired up.
TEST_CASE("smoke test - Catch2 is wired up", "[smoke]"){
    REQUIRE(1 + 1 == 2);
}
