#include "raylib.h"

// program main entry point
int main(void)
{
    int screenWidth = 800;
    int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    Vector2 shipPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    Color shipColor = DARKBLUE;
    
    SetTargetFPS(60);
    
    // main game loop
    while(!WindowShouldClose())
    {
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
        if(IsKeyDown(KEY_RIGHT)) shipPosition.x += 2.0f;
        if(IsKeyDown(KEY_LEFT)) shipPosition.x -= 2.0f;
        if(IsKeyDown(KEY_UP)) shipPosition.y -= 2.0f;
        if(IsKeyDown(KEY_DOWN)) shipPosition.y += 2.0f;
        //-----------------------------------------------------------------------------------------
        
        
        // Draw
        //-----------------------------------------------------------------------------------------
        BeginDrawing();
        
        {
            ClearBackground(RAYWHITE);
            
            /*
            DrawTriangleLines((Vector2){ screenWidth/2.0f, 160.0f },
                              (Vector2){ screenWidth/2.0f, 230.0f },
                              (Vector2){ screenWidth/2.0f, 230.0f }, shipColor);
            */
            
            DrawTriangleLines(Vector2{ shipPosition.x, shipPosition.y - 35.0f },
                              Vector2{ shipPosition.x - 20.0f, shipPosition.y + 35.0f },
                              Vector2{ shipPosition.x + 20.0f, shipPosition.y + 35.0f }, shipColor);
            
            DrawText("Congrats! You created your first window!", 10, 10, 20, DARKGRAY);
            DrawText("Press 'H' to toggle cursor visibitily", 10, 30, 20, DARKGRAY);
            
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