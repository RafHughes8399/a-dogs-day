#include "test_game.h"

#include <stdexcept>
#include <string>

// -----------------------------------------------------------------------------
// STUB bodies. Each non-lifecycle method currently throws so a half-finished
// harness can't produce false-green tests. Replace the todo(...) calls with the
// real implementation (the // impl: comments sketch the intended body).
// -----------------------------------------------------------------------------

namespace testing{

    namespace{
        [[noreturn]] void todo(const char* what){
            throw std::logic_error(std::string("test_game: not implemented yet - ") + what);
        }
    }

    // ---------------- lifecycle ----------------

    test_game::test_game() : level_(nullptr){
        // impl: SetConfigFlags(FLAG_WINDOW_HIDDEN); InitWindow(w, h, "test");
        //       level_ = level::level_builder().build_main_level();
        //       (after refactor) construct maitre_d_/expediter_ members.
    }

    test_game::~test_game(){
        // impl: drain events::global_dispatcher_'s pending queue, then let
        //       members destruct (RAII unsubscribe fires). CloseWindow().
    }

    // ---------------- simulation clock ----------------

    void test_game::tick(float /*delta*/, int /*frames*/){
        // impl: for each frame: process events, maitre_d_.update(delta),
        //       expediter_.process_orders().
        todo("tick");
    }

    bool test_game::tick_until(const std::function<bool()>& /*predicate*/, int /*max_frames*/){
        // impl: loop tick() up to max_frames, return true as soon as predicate()
        //       holds; false if it never does.
        todo("tick_until");
    }

    // ---------------- build actions ----------------

    std::unique_ptr<entities::entity> test_game::build_mack(int /*id*/, Vector2 /*position*/){
        // impl: return entities::e_builder.build_mack(position, id);
        todo("build_mack");
    }

    std::unique_ptr<entities::entity> test_game::build_khiri(int /*id*/, Vector2 /*position*/){
        // impl: return entities::e_builder.build_khiri(position, id);
        todo("build_khiri");
    }

    std::unique_ptr<entities::entity> test_game::build_customer_dog(
        int /*id*/, Vector2 /*position*/, std::optional<Vector2> /*destination*/, int /*dog_type*/){
        // impl: return entities::e_builder.build_customer_dog(id, dog_type, position, destination);
        todo("build_customer_dog");
    }

    std::unique_ptr<entities::entity> test_game::build_waiter_dog(
        int /*id*/, int /*dog_type*/, Vector2 /*position*/, std::optional<Vector2> /*destination*/){
        // impl: return entities::e_builder.build_waiter_dog(id, dog_type, position, destination);
        todo("build_waiter_dog");
    }

    // ---------------- insert actions ----------------

    void test_game::insert_entity(std::unique_ptr<entities::entity> /*entity*/, size_t /*layer*/){
        // impl: level_->add_entity(std::move(entity), layer);
        todo("insert_entity");
    }

    void test_game::insert_customer_dog(int /*id*/, Vector2 /*position*/, std::optional<Vector2> /*destination*/){
        // impl: insert_entity(build_customer_dog(id, position, destination), level_config::dogs);
        todo("insert_customer_dog");
    }

    void test_game::insert_waiter_dog(int /*id*/, int /*dog_type*/, Vector2 /*position*/, std::optional<Vector2> /*destination*/){
        // impl: insert_entity(build_waiter_dog(id, dog_type, position, destination), level_config::dogs);
        todo("insert_waiter_dog");
    }

    // ---------------- event triggers ----------------

    void test_game::customer_arrives(){
        // impl: maitre_d_.request_customer_arrival();
        todo("customer_arrives");
    }

    // ---------------- inspection accessors ----------------

    entities::entity* test_game::find_entity(int /*id*/){
        // impl: search level_'s entities for a matching id; nullptr if none.
        todo("find_entity");
    }

    entities::customer_dog& test_game::get_customer_dog(int /*id*/){
        // impl: dynamic_cast<customer_dog&> the result of find_entity(id).
        todo("get_customer_dog");
    }

} // namespace testing
