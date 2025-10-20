#include <iostream>
#include "raylib.h"
#include "raymath.h"

#include "config.h"
#include "game.h"
int main(){
    auto width = GetScreenWidth();
    auto height = GetScreenHeight();
    InitWindow(width, height, "dog");
    auto game = game::game();
    while(! WindowShouldClose()){
        float delta = GetFrameTime();
        game.update(delta);
        BeginDrawing();
            game.render(delta);
            game.debug(delta);
        EndDrawing();
    }
    /**
     * 
     auto background = LoadTexture(config::background_path);
     Vector2 background_position = Vector2Zero();
     auto view_frame = Rectangle{0.0f, 0.0f, static_cast<float>(GetScreenWidth()),static_cast<float>(GetScreenHeight())};
     Vector2 mouse_delta = Vector2Zero();
    // specify the frame being drawn
    
    while(! WindowShouldClose()){
        
    BeginDrawing();
    //ClearBackground(WHITE);
    //DrawTexture(background, 0, 0, WHITE);
    DrawTextureRec(background, view_frame, background_position, WHITE);
    ShowCursor();
    // trying to emulate some form of dragging
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        mouse_delta = GetMouseDelta();
    }
    else{
        mouse_delta = Vector2Zero();
    }
    auto frame_offset = Vector2Scale(mouse_delta, -1);
    // ! these values need to be clamped to avoid the repeating texture thing 
    view_frame.x +=  frame_offset.x;
    view_frame.y += frame_offset.y; 
    auto mouse_position = GetMousePosition();
    DrawText(TextFormat("mouse position: %f, %f", mouse_position.x, mouse_position.y), 100, 100, 14, BLUE);
    DrawText(TextFormat("mouse delta: %f, %f", mouse_delta.x, mouse_delta.y), 100, 120, 14, BLUE);
    DrawText(TextFormat("frame offset: %f, %f", frame_offset.x, frame_offset.y), 100, 140, 14, BLUE);
            EndDrawing();    
        }
    */
    CloseWindow();
    std::cout << " HELLO WORLD " << std::endl;
    return 0;
}