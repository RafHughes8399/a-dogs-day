cmake_minimum_required(VERSION 3.25) # FetchContent is available in 3.11+
project(dog-days)


# Generate compile_commands.json
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

add_library(raglib src/raglib.cpp)
link_libraries(raglib)

add_library(events src/events.cpp)
link_libraries(events)

add_library(queries src/queries.cpp)
link_libraries(queries)

add_library(animation src/animation.cpp)
link_libraries(animation)

add_library(sprite src/sprite.cpp)
link_libraries(sprite)

add_library(item src/items.cpp)
link_libraries(item)

add_library(menu src/menus.cpp)
link_libraries(menu)

add_library(hud src/hud.cpp)
link_libraries(hud)

add_library(hitbox src/hitbox.cpp)
link_libraries(hitbox)


add_library(texture src/texture.cpp)
link_libraries(texture)

add_library(player src/player.cpp)
link_libraries(player)

add_library(entities src/entities.cpp)
link_libraries(entities)

add_library(cursor src/cursor.cpp)
link_libraries(cursor)

add_library(dogs src/dogs.cpp)
link_libraries(dogs)

add_library(decorations src/decorations.cpp)
link_libraries(decorations)

add_library(render_layer src/render_layer.cpp)
link_libraries(render_layer)

add_library(quadtree src/quadtree.cpp)
link_libraries(quadtree)

add_library(level src/level.cpp)
link_libraries(level)

add_library(game src/game.cpp)
link_libraries(game)

add_executable(${PROJECT_NAME} src/dog_days.cpp)
target_link_libraries(${PROJECT_NAME} raylib)

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