#include <iostream>
#include "raylib.h"
int main(){
    auto width = GetScreenWidth();
    auto height = GetScreenHeight();
    InitWindow(width, height, "dog");
        while(! WindowShouldClose()){
            BeginDrawing();
                ClearBackground(WHITE);
            EndDrawing();    
        }
    CloseWindow();
    std::cout << " HELLO WORLD " << std::endl;
    return 0;
}