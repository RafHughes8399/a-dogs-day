#include "game.h"
#include "raylib.h"
#include <algorithm>
#include <iostream>


int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(game_config::window_width, game_config::window_height, "dog day");
  ToggleBorderlessWindowed();
  HideCursor();
  SetTargetFPS(60);

  const float base_width = level_config::screen_width;
  const float base_height = level_config::screen_height;
  RenderTexture2D canvas = LoadRenderTexture(game_config::window_width, game_config::window_height);
  SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

  bool loop = true;

  std::cout << "[dog_days init] : start loop " << std::endl;
  auto game = game::game();
  game.init();
  while (loop) {
    float delta = GetFrameTime();

    float scale = std::min(static_cast<float>(GetScreenWidth()) / base_width,
                           static_cast<float>(GetScreenHeight()) / base_height);
    Vector2 offset{(static_cast<float>(GetScreenWidth()) - base_width * scale) * 0.5f,
                   (static_cast<float>(GetScreenHeight()) - base_height * scale) * 0.5f};
    SetMouseOffset(static_cast<int>(-offset.x), static_cast<int>(-offset.y));
    SetMouseScale(1.0f / scale, 1.0f / scale);

    game.update(delta);

    BeginTextureMode(canvas);
    ClearBackground(BLACK);
    game.render(delta);
    game.debug(delta);
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(canvas.texture, Rectangle{0.0f, 0.0f, base_width, -base_height},
                   Rectangle{offset.x, offset.y, base_width * scale, base_height * scale},
                   Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
    if (IsKeyPressed(KEY_PERIOD) or WindowShouldClose()) {
      loop = false;
    }
  }
  UnloadRenderTexture(canvas);
  CloseWindow();
  std::cout << " HELLO WORLD " << std::endl; // 20 / 10 / 25
  return 0;
}
