#include <iostream>
#include "raylib.h"
int main(){
    auto width = GetScreenWidth();
    auto height = GetScreenHeight();
    InitWindow(width, height, "dog");
        while(! WindowShouldClose()){
            BeginDrawing();
                ClearBackground(WHITE);
                //ShowCursor();
                auto mouse_position = GetMousePosition();
                DrawText(TextFormat("mouse position: %f, %f", mouse_position.x, mouse_position.y), 100, 100, 14, BLUE);
            EndDrawing();    
        }
    CloseWindow();
    std::cout << " HELLO WORLD " << std::endl;
    return 0;
}