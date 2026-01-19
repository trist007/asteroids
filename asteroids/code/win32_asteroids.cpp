#include "raylib.h"
#include "raymath.h"

#define MAX_BULLETS 20
#define MAX_ASTEROIDS 8

// Program main entry point
int main(void)
{
    int screenWidth = 1024;
    int screenHeight = 768;
    
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    SetTargetFPS(60);
    
    // Variables
    
    bool gameOver = false;
    
    // Timer
    float gameTimer = 0.0f;
    float speedIncreaseInterval = 10.0f;
    float asteroidSpeedMultiplier = 1.0f;
    
    float asteroidSpawnTimer = 0.0f;
    float asteroidSpawnInterval = 1.0f;
    
    // Player ship
    Vector2 shipPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    Vector2 shipVelocity = { 0.0f, 0.0f };
    float thrust = 0.1f;
    float friction = 0.99f; // 1.0 for no friction, lower = more friction
    Color shipColor = DARKBLUE;
    float shipRotation = 0.0f;
    float shipSize = 15.0f;
    bool crossedOver = false;
    
    // Getting radius of screen windows for asteroid spawn
    float screenRadius = sqrtf((screenWidth * screenWidth) + (screenHeight * screenHeight)) / 2;
    
    // Asteroid
    Vector2 asteroidPos[MAX_ASTEROIDS];
    Vector2 asteroidVelocity[MAX_ASTEROIDS];
    Vector2 asteroidDirection[MAX_ASTEROIDS];
    int asteroidScale[MAX_ASTEROIDS];
    bool asteroidSpawned[MAX_ASTEROIDS];
    float asteroidSpeed = 2.0f;
    Vector2 asteroidTarget = { screenWidth / 2.0f, screenHeight / 2.0f };
    
    // Bullets
    Vector2 bulletPosition[MAX_BULLETS];
    Vector2 bulletVelocity[MAX_BULLETS];
    bool bulletActive[MAX_BULLETS];
    float bulletRadius = 3.0f;
    float bulletSpeed = 10.0f;
    
    // Initialize bullets
    for(int i = 0;
        i < MAX_BULLETS;
        i++)
    {
        bulletActive[i] = false;
    }
    
    // Initialize asteroids
    for(int i = 0;
        i < MAX_ASTEROIDS;
        i++)
    {
        asteroidSpawned[i] = false;
    }
    
    // Main game loop
    while(!WindowShouldClose())
    {
        //-----------------------------------------------------------------------------------------
        // Update
        //-----------------------------------------------------------------------------------------
        if(!gameOver)
        {
            if(IsKeyPressed(KEY_H))
            {
                if(IsCursorHidden()) ShowCursor();
                else HideCursor();
            }
            
            // Control ship with keyboard
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
            
            // Adjust asteroid speed
            gameTimer += GetFrameTime();
            if(gameTimer >= speedIncreaseInterval)
            {
                asteroidSpeedMultiplier += 0.5f;
                gameTimer = 0.0f;
            }
            
            // Apply velocity and friction to shipPosition
            shipPosition.x += shipVelocity.x;
            shipPosition.y += shipVelocity.y;
            
            shipVelocity.x *= friction;
            shipVelocity.y *= friction;
            
            // Update active bullets
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
            
            asteroidSpawnTimer += GetFrameTime();
            
            if(asteroidSpawnTimer >= asteroidSpawnInterval)
            {
                
                // Spawn asteroids
                for(int i = 0;
                    i < MAX_ASTEROIDS;
                    i++)
                {
                    if(!asteroidSpawned[i])
                    {
                        // Get random angle and scale for asteroid
                        float angle = GetRandomValue(0, 360) * DEG2RAD;
                        float spawnRadius = screenRadius + 50.0f;
                        
                        asteroidPos[i].x = screenWidth / 2.0f + cosf(angle) * spawnRadius;
                        asteroidPos[i].y = screenHeight / 2.0f + sinf(angle) * spawnRadius;
                        
                        asteroidScale[i] = GetRandomValue(10, 80);
                        asteroidSpawned[i] = true;
                        asteroidDirection[i] = Vector2Normalize(Vector2Subtract(asteroidTarget, asteroidPos[i]));
                        asteroidVelocity[i] = Vector2Scale(asteroidDirection[i], asteroidSpeed * asteroidSpeedMultiplier);
                        
                        // Only spawn one asteroid per interval
                        asteroidSpawnTimer = 0.0f;
                        break;
                    }
                }
            }
            
            // Update spawned asteroids
            for(int i = 0;
                i < MAX_ASTEROIDS;
                i++)
            {
                if(asteroidSpawned[i])
                {
                    asteroidPos[i].x += asteroidVelocity[i].x;
                    asteroidPos[i].y += asteroidVelocity[i].y;
                    
                    float margin = 200.0f;
                    if(asteroidPos[i].x < -margin || asteroidPos[i].x > screenWidth + margin ||
                       asteroidPos[i].y < -margin || asteroidPos[i].y > screenHeight + margin)
                    {
                        asteroidSpawned[i] = false;
                    }
                }
            }
            
            // Check for bullet asteroid collisions
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(bulletActive[i])
                {
                    for(int j = 0;
                        j< MAX_ASTEROIDS;
                        j++)
                    {
                        if(asteroidSpawned[j])
                        {
                            float distance = Vector2Distance(bulletPosition[i], asteroidPos[j]);
                            
                            if(distance < (asteroidScale[j] + bulletRadius))
                            {
                                bulletActive[i] = false;
                                asteroidSpawned[j] = false;
                            }
                        }
                    }
                }
            }
            
            // Check for asteroid player collisions
            if(!gameOver)
            {
                for(int i = 0;
                    i < MAX_ASTEROIDS;
                    i++)
                {
                    if(asteroidSpawned[i])
                    {
                        float distance = Vector2Distance(asteroidPos[i], shipPosition);
                        if(distance < (asteroidScale[i] + shipSize))
                        {
                            gameOver = true;
                            break;
                        }
                    }
                }
            }
            
            // Check if player ship has gone offscreen only to wrap on the opposite end
            // NOTE(trist007): if the ship moves very fast it can do multiple
            // wraps so you can use the crossedOver bool
            if(shipPosition.x < 0 || shipPosition.x > screenWidth ||
               shipPosition.y < 0 || shipPosition.y > screenHeight)
            {
                // Wrap horizontally
                if(shipPosition.x < 0) shipPosition.x = screenWidth;
                if(shipPosition.x > screenWidth) shipPosition.x = 0;
                
                // Wrap veritcally
                if(shipPosition.y < 0) shipPosition.y = screenHeight;
                if(shipPosition.y > screenHeight) shipPosition.y = 0;
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
            
            // Draw asteroids
            for(int i = 0;
                i < MAX_ASTEROIDS;
                i++)
            {
                if(asteroidSpawned[i])
                {
                    DrawCircleV(asteroidPos[i], asteroidScale[i], GRAY);
                }
            }
            
            if(IsCursorHidden()) DrawText("CURSOR HIDDEN", 20, 60, 20, RED);
            else DrawText("CURSOR VISIBLE", 20, 60, 20, LIME);
            
        }
        
        if(gameOver)
        {
            int fontSize = 80;
            char *text = "GAME OVER";
            int textWidth = MeasureText(text, fontSize);
            DrawText(text, screenWidth / 2 - textWidth / 2, screenHeight / 2 - fontSize / 2, fontSize, RED);
        }
        
        EndDrawing();
        //-----------------------------------------------------------------------------------------
    }
    
    // De-Initialization
    CloseWindow();
    
    return(0);
}