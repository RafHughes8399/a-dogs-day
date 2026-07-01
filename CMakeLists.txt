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

# Our Project

# add_library(lib_name src/lib_name.cpp)
# link_libraries(lib_name)

link_libraries(raylib)

add_library(raglib src/engine/raglib.cpp)
link_libraries(raglib)

add_library(events src/systems/events.cpp)
link_libraries(events)

add_library(queries src/systems/queries.cpp)
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

add_library(entities src/entities/entities.cpp)
link_libraries(entities)

add_library(cursor src/entities/cursor.cpp)
link_libraries(cursor)

add_library(dogs src/entities/dogs.cpp)
link_libraries(dogs)

add_library(decorations src/entities/decorations.cpp)
link_libraries(decorations)

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

add_library(maitre_d src/systems/maitre_d.cpp src/systems/maitre_d_interface.cpp src/systems/dog_queue.cpp)
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
  player controls entities cursor dogs decorations render_layer quadtree
  graph level maitre_d expediter debug_log_interface debug_logger game ${PROJECT_NAME}
)
foreach(target IN LISTS PROJECT_TARGETS)
  target_compile_options(${target} PRIVATE ${MY_COMPILE_OPTIONS})
  target_include_directories(${target} PRIVATE
    src/rendering
    src/entities
    src/systems
    src/engine
    src/interfaces
  )
  target_include_directories(${target} SYSTEM PRIVATE ${raylib_SOURCE_DIR}/src)
endforeach()

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
endif()
