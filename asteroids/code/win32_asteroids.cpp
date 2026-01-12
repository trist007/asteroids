#include "raylib.h"

// program main entry point
int main(void)
{
    int screenWidth = 800;
    int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    Vector2 ballPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    
    SetTargetFPS(60);
    
    // main game loop
    while(!WindowShouldClose())
    {
        // Update
        //-----------------------------------------------------------------------------------------
        
        if(IsKeyDown(KEY_RIGHT)) ballPosition.x += 2.0f;
        if(IsKeyDown(KEY_LEFT)) ballPosition.x -= 2.0f;
        if(IsKeyDown(KEY_UP)) ballPosition.y -= 2.0f;
        if(IsKeyDown(KEY_DOWN)) ballPosition.y += 2.0f;
        //-----------------------------------------------------------------------------------------
        
        
        // Draw
        //-----------------------------------------------------------------------------------------
        BeginDrawing();
        
        {
            ClearBackground(RAYWHITE);
            
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            
            DrawCircleV(ballPosition, 50, MAROON);
        }
        
        EndDrawing();
        //-----------------------------------------------------------------------------------------
    }
    
    // De-Initialization
    CloseWindow();
    
    return(0);
}