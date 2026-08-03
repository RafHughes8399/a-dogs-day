#include "test_game.h"

#include "events.h" // events::global_dispatcher_

#include <stdexcept>
#include <string>

// -----------------------------------------------------------------------------
// Implemented incrementally (spec: test-game-catch2-design.md). Lifecycle + the
// player-dog / decoration / food build+insert+inspect actions are live; the
// remaining flow methods (customer/waiter dogs) still throw todo(...) so a
// half-finished harness can't produce false-green tests.
// -----------------------------------------------------------------------------

namespace testing{

    namespace{
        [[noreturn]] void todo(const char* what){
            throw std::logic_error(std::string("test_game: not implemented yet - ") + what);
        }
    }

    // ---------------- lifecycle ----------------

    test_game::test_game() : level_(nullptr){
        // A single hidden window is kept open for the whole test run so the
        // production entity builders can LoadTexture() and the shared texture
        // cache stays valid across scenarios. (Spec sketches per-scenario
        // Init/CloseWindow; we keep one process-global window instead to avoid
        // repeated GL init and stale cached texture handles - see ~test_game.)
        if(not IsWindowReady()){
            SetConfigFlags(FLAG_WINDOW_HIDDEN);
            InitWindow(1, 1, "dog-days tests");
        }
        level_ = level::level_builder().build_main_level();
    }

    test_game::~test_game(){
        // Drain the shared dispatcher queue so events queued this scenario (e.g.
        // registered_* from build_main_level / inserts) don't leak into the next.
        events::global_dispatcher_.process_events(0.0f);
        // Destroy the level (and its entities) now; maitre_d_/expediter_ value
        // members unsubscribe via RAII when this object finishes destructing.
        level_.reset();
        // Window intentionally left open for the process lifetime (see ctor); the
        // OS reclaims it at test-binary exit.
    }

    // ---------------- simulation clock ----------------

    void test_game::tick(float delta, int frames){
        // Mirror game::update's system order, minus player/controls/menus/render.
        for(int frame = 0; frame < frames; ++frame){
            events::global_dispatcher_.process_events(delta);
            maitre_d_.update(delta);
            expediter_.process_serving_jobs();
            expediter_.process_clearing_jobs();
            level_->update(delta, frame);
        }
    }

    bool test_game::tick_until(const std::function<bool()>& predicate, int max_frames){
        for(int frame = 0; frame < max_frames; ++frame){
            if(predicate()){
                return true;
            }
            tick(1.0f / 60.0f, 1);
        }
        return predicate();
    }

    // ---------------- build actions ----------------

    std::unique_ptr<entities::entity> test_game::build_mack(int id, Vector2 position){
        return entities::e_builder.build_mack(position, id);
    }

    std::unique_ptr<entities::entity> test_game::build_khiri(int id, Vector2 position){
        return entities::e_builder.build_khiri(position, id);
    }

    std::unique_ptr<entities::entity> test_game::build_table(int id, Vector2 position){
        return entities::e_builder.build_table(position, id);
    }

    std::unique_ptr<entities::entity> test_game::build_food_counter(int id, Vector2 position){
        return entities::e_builder.build_food_counter(position, id);
    }

    std::unique_ptr<entities::entity> test_game::build_dishwasher(int id, Vector2 position){
        return entities::e_builder.build_dishwasher(position, id);
    }

    std::unique_ptr<entities::food> test_game::build_test_food(int id, Vector2 position){
        return entities::e_builder.build_test_food(position, id);
    }

    std::unique_ptr<entities::entity> test_game::build_customer_dog(
        int id, Vector2 position, std::optional<Vector2> destination, int dog_type){
        return entities::e_builder.build_customer_dog(id, dog_type, position, destination);
    }

    std::unique_ptr<entities::entity> test_game::build_waiter_dog(
        int id, int dog_type, Vector2 position, std::optional<Vector2> destination){
        return entities::e_builder.build_waiter_dog(id, dog_type, position, destination);
    }

    std::unique_ptr<entities::entity> test_game::build_dishwasher_dog(int id, Vector2 position){
        return entities::e_builder.build_dishwasher_dog(id, position);
    }

    // ---------------- insert actions ----------------

    void test_game::insert_entity(std::unique_ptr<entities::entity> entity, size_t layer){
        level_->add_entity(std::move(entity), layer);
    }

    void test_game::insert_customer_dog(int id, Vector2 position, std::optional<Vector2> destination){
        insert_entity(build_customer_dog(id, position, destination), level_config::draw_layers::dogs);
    }

    void test_game::insert_waiter_dog(int id, int dog_type, Vector2 position, std::optional<Vector2> destination){
        insert_entity(build_waiter_dog(id, dog_type, position, destination), level_config::draw_layers::dogs);
    }

    void test_game::insert_dishwasher_dog(int id, Vector2 position){
        insert_entity(build_dishwasher_dog(id, position), level_config::draw_layers::dogs);
    }

    void test_game::insert_dishwasher(int id, Vector2 position){
        insert_entity(build_dishwasher(id, position), level_config::draw_layers::stations);
    }

    void test_game::insert_table(int id, Vector2 position){
        insert_entity(build_table(id, position), level_config::draw_layers::stations);
    }

    void test_game::insert_food_counter(int id, Vector2 position){
        insert_entity(build_food_counter(id, position), level_config::draw_layers::stations);
    }

    // ---------------- event triggers ----------------

    void test_game::customer_arrives(){
        // Queues build_customer_dog; the level builds the entity and fires
        // customer_dog_created, so the customer only exists after a tick.
        maitre_d_.request_customer_arrival();
    }

    void test_game::request_order(size_t customer_id, size_t table_id, Vector2 table_position){
        std::unique_ptr<events::event> reached = std::make_unique<events::dog_reached_station>(
            customer_id, table_id, table_position);
        event_interface::queue_event(reached);
    }

    void test_game::remove_entity(int id){
        events::remove_entity remove{static_cast<size_t>(id)};
        event_interface::execute_event(remove);
    }

    void test_game::move_entity(int id, Vector2 position){
        auto* entity = find_entity(id);
        if(entity == nullptr){
            todo("move_entity: id not present");
        }
        entity->move(position);
    }

    void test_game::request_clear_table(int table_id){
        auto* table = find_table(table_id);
        if(table == nullptr){
            todo("request_clear_table: id is not a table / not present");
        }
        events::clear_table clear{table};
        event_interface::execute_event(clear);
    }

    // ---------------- inspection accessors ----------------

    entities::entity* test_game::find_entity(int id){
        return level_->get_entity(id);
    }

    int test_game::num_entities(){
        return level_->num_entities();
    }

    int test_game::num_waiters(){
        return static_cast<int>(expediter_.num_waiters());
    }

    int test_game::num_counters(){
        return static_cast<int>(expediter_.num_counters());
    }

    int test_game::num_tables(){
        return static_cast<int>(maitre_d_.num_tables());
    }

    int test_game::num_customers(){
        return static_cast<int>(maitre_d_.num_customers());
    }

    int test_game::num_expediter_tables(){
        return static_cast<int>(expediter_.num_tables());
    }

    int test_game::num_dishwashers(){
        return static_cast<int>(expediter_.num_dishwashers());
    }

    int test_game::num_serving_jobs(){
        return static_cast<int>(expediter_.num_serving_jobs());
    }

    int test_game::num_clearing_jobs(){
        return static_cast<int>(expediter_.num_clearing_jobs());
    }

    int test_game::num_tracked_customers(){
        return static_cast<int>(maitre_d_.num_tracked_customers());
    }

    expediter::serving_job_status test_game::first_serving_job_status(){
        return expediter_.first_serving_job_status();
    }

    entities::waiter_dog* test_game::first_waiter(){
        return expediter_.first_waiter();
    }

    entities::food_counter* test_game::first_counter(){
        return expediter_.first_counter();
    }

    entities::dishwasher* test_game::first_dishwasher(){
        return expediter_.first_dishwasher();
    }

    entities::customer_dog* test_game::first_customer(){
        return maitre_d_.first_customer();
    }

    entities::table* test_game::find_table(int id){
        return dynamic_cast<entities::table*>(find_entity(id));
    }

    entities::dishwasher* test_game::find_dishwasher(int id){
        return dynamic_cast<entities::dishwasher*>(find_entity(id));
    }

    entities::customer_dog& test_game::get_customer_dog(int id){
        auto* customer = dynamic_cast<entities::customer_dog*>(find_entity(id));
        if(customer == nullptr){
            todo("get_customer_dog: id is not a customer dog / not present");
        }
        return *customer;
    }

    entities::waiter_dog& test_game::get_waiter_dog(int id){
        auto* waiter = dynamic_cast<entities::waiter_dog*>(find_entity(id));
        if(waiter == nullptr){
            todo("get_waiter_dog: id is not a waiter dog / not present");
        }
        return *waiter;
    }

} // namespace testing
