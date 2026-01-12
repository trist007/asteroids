#include "raylib.h"
#include <math.h>

// program main entry point
int main(void)
{
    int screenWidth = 1024;
    int screenHeight = 768;
    
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    
    // Variables
    Vector2 shipPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    Vector2 shipVelocity = { 0.0f, 0.0f };
    float thrust = 0.1f;
    float friction = 0.99f; // 1.0 for no friction, lower = more friction
    Color shipColor = DARKBLUE;
    float shipRotation = 0.0f;
    float shipSize = 15.0f;
    
    SetTargetFPS(60);
    
    // main game loop
    while(!WindowShouldClose())
    {
        //-----------------------------------------------------------------------------------------
        // Update
        //-----------------------------------------------------------------------------------------
        if(IsKeyPressed(KEY_H))
        {
            if(IsCursorHidden()) ShowCursor();
            else HideCursor();
        }
        
        /* control cirlce with the mouse
        ballPosition = GetMousePosition();
        
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ballColor = MAROON;
        else if(IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) ballColor = LIME;
        else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) ballColor = DARKBLUE;
        else if(IsMouseButtonPressed(MOUSE_BUTTON_SIDE)) ballColor = PURPLE;
        else if(IsMouseButtonPressed(MOUSE_BUTTON_EXTRA)) ballColor = YELLOW;
        else if(IsMouseButtonPressed(MOUSE_BUTTON_FORWARD)) ballColor = ORANGE;
        else if(IsMouseButtonPressed(MOUSE_BUTTON_BACK)) ballColor = BEIGE;
*/
        
        // control ship with keyboard
        if(IsKeyDown(KEY_RIGHT)) shipRotation += 0.05f;
        if(IsKeyDown(KEY_LEFT)) shipRotation -= 0.05f;
        if(IsKeyDown(KEY_UP))
        {
            shipVelocity.x += sinf(shipRotation) * thrust;
            shipVelocity.y -= cosf(shipRotation) * thrust;
        }
        if(IsKeyDown(KEY_DOWN))
        {
            shipVelocity.x -= sinf(shipRotation) * thrust;
            shipVelocity.y += cosf(shipRotation) * thrust;
        }
        
        // Apply velocity and friction to shipPosition
        shipPosition.x += shipVelocity.x;
        shipPosition.y += shipVelocity.y;
        
        shipVelocity.x *= friction;
        shipVelocity.y *= friction;
        
        //-----------------------------------------------------------------------------------------
        // Draw
        //-----------------------------------------------------------------------------------------
        BeginDrawing();
        
        {
            ClearBackground(RAYWHITE);
            
            Vector2 v1 = {
                shipPosition.x + sinf(shipRotation) * shipSize,
                shipPosition.y - cosf(shipRotation) * shipSize
            };
            
            Vector2 v2 = {
                shipPosition.x + sinf(shipRotation + 2.4f) * shipSize,
                shipPosition.y - cosf(shipRotation + 2.4f) * shipSize
            };
            
            Vector2 v3 = {
                shipPosition.x + sinf(shipRotation - 2.4f) * shipSize,
                shipPosition.y - cosf(shipRotation - 2.4f) * shipSize
            };
            
            DrawTriangle(v1, v3, v2, shipColor);
            DrawTriangleLines(v1, v3, v2, BLACK);
            
            
            if(IsCursorHidden()) DrawText("CURSOR HIDDEN", 20, 60, 20, RED);
            else DrawText("CURSOR VISIBLE", 20, 60, 20, LIME);
            
        }
        
        EndDrawing();
        //-----------------------------------------------------------------------------------------
    }
    
    // De-Initialization
    CloseWindow();
    
    return(0);
}