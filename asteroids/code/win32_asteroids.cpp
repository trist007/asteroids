#include "raylib.h"

// program main entry point
int main(void)
{
    int screenWidth = 800;
    int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    SetTargetFPS(60);
    
    // main game loop
    while(!WindowShouldClose())
    {
        // update
        // update variables here
        // draw
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            
        }
        EndDrawing();
    }
    
    // de-initialization
    CloseWindow();
    
    return(0);
}