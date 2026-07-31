cmake_minimum_required(VERSION 3.25) # FetchContent is available in 3.11+
project(dog-days)


# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# define compile options
set(MY_COMPILE_OPTIONS
  -Wall
  -Wextra
  #-Werror
  -pedantic-errors
  -Wconversion
  -Wcast-align
  -Wformat=2
  -Wuninitialized
  -Wnull-dereference
  -Wnon-virtual-dtor
  -Woverloaded-virtual
  -Wdeprecated-copy-dtor
  -Wold-style-cast
  -Wzero-as-null-pointer-constant
  -Wsuggest-override
  -fstack-protector-strong
  -Wshadow
  -Wimplicit-fallthrough
  -Wundef
  -Wcast-qual
  -Wconditional-uninitialized
  -Wunused-lambda-capture
  -Wloop-analysis
  -Wmove
  -Wself-assign
  -Wself-move
  -Wdangling
  -Wdelete-non-virtual-dtor
  -Wsuggest-destructor-override
  -Winconsistent-missing-destructor-override
  -Wreturn-std-move
  -Wtautological-compare
  -Wvla
  -Wduplicate-enum
  -Wextra-semi
  -Wdeprecated
  -Wsign-conversion
  -Wfloat-equal
  # Disable problematic warnings for external headers
  -Wno-double-promotion
  -Wno-missing-field-initializers
)
# setup raylib
# Dependencies
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

# setup Catch2 (unit test framework)
# Dependencies
set(CATCH2_VERSION 3.15.1)
find_package(Catch2 3 QUIET) # QUIET or REQUIRED
if (NOT Catch2_FOUND) # If there's none, fetch and build Catch2
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v${CATCH2_VERSION}.tar.gz
  )
  FetchContent_GetProperties(Catch2)
  if (NOT catch2_POPULATED) # Have we downloaded Catch2 yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(Catch2)
  endif()
endif()

# Our Project

# add_library(lib_name src/lib_name.cpp)
# link_libraries(lib_name)

link_libraries(raylib)

add_library(raglib src/engine/raglib.cpp)
link_libraries(raglib)

add_library(events src/systems/events/events.cpp)
link_libraries(events)

add_library(queries src/systems/queries/queries.cpp)
link_libraries(queries)

add_library(animation src/rendering/animation.cpp)
link_libraries(animation)

add_library(sprite src/rendering/sprite.cpp)
link_libraries(sprite)

add_library(item src/entities/items.cpp)
link_libraries(item)

add_library(menu src/interfaces/menus.cpp)
link_libraries(menu)

add_library(hud src/interfaces/hud.cpp)
link_libraries(hud)

add_library(hitbox src/entities/hitbox.cpp)
link_libraries(hitbox)

add_library(body src/rendering/body.cpp)
link_libraries(body)

add_library(texture src/rendering/texture.cpp)
link_libraries(texture)

add_library(player src/interfaces/player.cpp)
link_libraries(player)

add_library(controls src/interfaces/controls.cpp)
link_libraries(controls)

add_library(entities src/entities/entity.cpp src/entities/entity_builder.cpp)
link_libraries(entities)

add_library(cursor src/entities/cursor.cpp src/entities/cursor_builder.cpp)
link_libraries(cursor)

add_library(dogs
  src/entities/dogs/dog.cpp
  src/entities/dogs/player_dog.cpp
  src/entities/dogs/customer_dog.cpp
  src/entities/dogs/waiter_dog.cpp
  src/entities/dogs/player_dog_builder.cpp
  src/entities/dogs/customer_dog_builder.cpp
  src/entities/dogs/waiter_dog_builder.cpp
  src/entities/dogs/dishwasher_dog_builder.cpp)
link_libraries(dogs)

add_library(decorations
  src/entities/decorations/decoration.cpp
  src/entities/decorations/decoration_builder.cpp)
link_libraries(decorations)

add_library(stations
  src/entities/stations/station.cpp
  src/entities/stations/table.cpp
  src/entities/stations/food_counter.cpp
  src/entities/stations/table_builder.cpp
  src/entities/stations/food_counter_builder.cpp
  src/entities/stations/dishwasher_builder.cpp)
link_libraries(stations)

add_library(food src/entities/food/food_builder.cpp)
link_libraries(food)

add_library(render_layer src/engine/render_layer.cpp)
link_libraries(render_layer)

add_library(quadtree src/engine/quadtree.cpp)
link_libraries(quadtree)

add_library(graph src/engine/graph.cpp)
link_libraries(graph)

add_library(level src/engine/level.cpp)
link_libraries(level)

add_library(dog_actions src/systems/dog_actions.cpp)
link_libraries(dog_actions)

add_library(maitre_d src/systems/maitre_d.cpp src/systems/dog_queue.cpp)
link_libraries(maitre_d)

add_library(expediter src/systems/expediter.cpp)
link_libraries(expediter)

add_library(debug_log_interface src/systems/debug_log_interface.cpp)
link_libraries(debug_log_interface)

add_library(debug_logger src/systems/debug_logger.cpp)
link_libraries(debug_logger)

add_library(game src/engine/game.cpp)
link_libraries(game)


add_executable(${PROJECT_NAME} src/dog_days.cpp)

target_link_libraries(${PROJECT_NAME} raylib)
set(PROJECT_TARGETS
  raglib events queries animation sprite item menu hud hitbox body texture
  player controls entities cursor dogs decorations stations food render_layer quadtree
  graph level maitre_d expediter dog_actions debug_log_interface debug_logger game ${PROJECT_NAME}
)
foreach(target IN LISTS PROJECT_TARGETS)
  target_compile_options(${target} PRIVATE ${MY_COMPILE_OPTIONS})
  target_include_directories(${target} PRIVATE
    src/rendering
    src/entities
    src/entities/dogs
    src/entities/decorations
    src/entities/stations
    src/entities/food
    src/systems
    src/systems/events
    src/systems/queries
    src/engine
    src/interfaces
  )
  target_include_directories(${target} SYSTEM PRIVATE ${raylib_SOURCE_DIR}/src)
endforeach()

# Tests
# Catch2 links ONLY here, never into the shipped dog-days binary.
# Catch2::Catch2WithMain provides the test runner's main().
# The game's static libs are already attached via the global link_libraries()
# calls above; here we add the test sources, headers, and the test-only macro.
add_executable(tests
  tests/smoke_test.cpp
  tests/test_game.cpp
  tests/harness_scenarios.cpp
  tests/player_scenarios.cpp
  tests/decoration_scenarios.cpp
  tests/food_scenarios.cpp
  tests/maitre_d_scenarios.cpp
  tests/expediter_scenarios.cpp
  tests/station_scenarios.cpp
  tests/dishwasher_dog_scenarios.cpp
  tests/dishwasher_scenarios.cpp
  tests/customer_dog_scenarios.cpp
  tests/waiter_dog_scenarios.cpp
  tests/quadtree_scenarios.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
# Test code needs the game's headers (and raylib), but deliberately NOT the
# strict MY_COMPILE_OPTIONS - those would fire inside Catch2 / raylib headers.
target_include_directories(tests PRIVATE
  src/rendering
  src/entities
  src/entities/dogs
  src/entities/decorations
  src/entities/stations
  src/entities/food
  src/systems
  src/systems/events
  src/systems/queries
  src/engine
  src/interfaces
)
target_include_directories(tests SYSTEM PRIVATE ${raylib_SOURCE_DIR}/src)
# Compile the DOG_DAYS_TESTING-guarded accessors (spec step 5) into this
# target ONLY, so they never reach the shipped dog-days binary.
target_compile_definitions(tests PRIVATE DOG_DAYS_TESTING)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")

    # The global link_libraries() calls above append every static lib to each
    # target's link line, so the executables list many of them more than once.
    # Apple's new linker (ld-prime) then warns "ignoring duplicate libraries".
    # The duplicates are already ignored and harmless; silence the noise. This
    # flag exists only on the new linker, which is also the one that warns.
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,-no_warn_duplicate_libraries)
    target_link_options(tests PRIVATE -Wl,-no_warn_duplicate_libraries)
endif()
