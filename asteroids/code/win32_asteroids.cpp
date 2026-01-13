#include "raylib.h"
#include <math.h>

#define MAX_BULLETS 20

// program main entry point
int main(void)
{
    int screenWidth = 1024;
    int screenHeight = 768;
    
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    SetTargetFPS(60);
    
    // Variables
    
    // Player ship
    Vector2 shipPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    Vector2 shipVelocity = { 0.0f, 0.0f };
    float thrust = 0.1f;
    float friction = 0.99f; // 1.0 for no friction, lower = more friction
    Color shipColor = DARKBLUE;
    float shipRotation = 0.0f;
    float shipSize = 15.0f;
    
    // Bullets
    Vector2 bulletPosition[MAX_BULLETS];
    Vector2 bulletVelocity[MAX_BULLETS];
    bool bulletActive[MAX_BULLETS];
    float bulletSpeed = 10.0f;
    
    // Initialize bullets
    for(int i = 0;
        i < MAX_BULLETS;
        i++)
    {
        bulletActive[i] = false;
    }
    
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
        if(IsKeyPressed(KEY_SPACE))
        {
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(!bulletActive[i])
                {
                    bulletActive[i] = true;
                    bulletPosition[i] = shipPosition;
                    bulletVelocity[i].x = sinf(shipRotation) * bulletSpeed;
                    bulletVelocity[i].y = -cosf(shipRotation) * bulletSpeed;
                    break;
                }
            }
        }
        
        // Apply velocity and friction to shipPosition
        shipPosition.x += shipVelocity.x;
        shipPosition.y += shipVelocity.y;
        
        shipVelocity.x *= friction;
        shipVelocity.y *= friction;
        
        // Update existing bullets
        for(int i = 0;
            i < MAX_BULLETS;
            i++)
        {
            if(bulletActive[i])
            {
                bulletPosition[i].x += bulletVelocity[i].x;
                bulletPosition[i].y += bulletVelocity[i].y;
                
                // Deactive if off screen
                if(bulletPosition[i].x < 0 || bulletPosition[i].x > screenWidth ||
                   bulletPosition[i].y < 0 || bulletPosition[i].y > screenHeight)
                {
                    bulletActive[i] = false;
                }
            }
        }
        
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
            
            // Draw bullets
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(bulletActive[i])
                {
                    DrawCircleV(bulletPosition[i], 3.0f, RED);
                }
            }
            
            
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