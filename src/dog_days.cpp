#include "game.h"
#include "raylib.h"
#include <iostream>


int main() {
  InitWindow(game_config::window_width, game_config::window_height, "dog day");
  HideCursor();
  SetTargetFPS(60);
  bool loop = true;

  std::cout << "[dog_days init] : start loop " << std::endl;
  auto game = game::game();
  game.init();
  while (loop) {
    float delta = GetFrameTime();
    BeginDrawing();
    game.update(delta);
    game.render(delta);
    game.debug(delta);
    EndDrawing();
    if (IsKeyPressed(KEY_PERIOD) or WindowShouldClose()) {
      loop = false;
    }
  }
  CloseWindow();
  std::cout << " HELLO WORLD " << std::endl; // 20 / 10 / 25
  return 0;
}
