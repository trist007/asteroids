#include "raylib.h"
#include "raymath.h"

#define MAX_BULLETS 20
#define MAX_LARGE_ASTEROIDS 8
#define MAX_SMALL_ASTEROIDS 12

int screenWidth = 1024;
int screenHeight = 768;

// Getting radius of screen windows for asteroid spawn
float screenRadius = sqrtf((screenWidth * screenWidth) + (screenHeight * screenHeight)) / 2;

// Spawn margin so asteroid will not spawn within the screen
float spawnMargin = 50.0f;

// Asteroid attributes
float asteroidSpeed = 2.0f;
Vector2 asteroidTarget = { screenWidth / 2.0f, screenHeight / 2.0f };

// Implement difficulty where every 10 seconds 0.2f gets added
float asteroidSpeedMultiplier = 1.0f;

// size of bullet
float bulletRadius = 3.0f;
float bulletSpeed = 10.0f;

typedef struct
{
    Vector2 pos;
    Vector2 velocity;
    Color color;
    float thrust;
    float friction;
    float rotation;
    float size;
} Ship;

typedef struct
{
    Vector2 pos;
    Vector2 velocity;
    bool active;
} Bullet;

typedef struct
{
    Vector2 pos;
    Vector2 velocity;
    Vector2 direction;
    int size;
    bool active;
} Asteroid;

void spawnSmallAsteroid(Asteroid *smallAsteroids, Vector2 asteroidPos, Vector2 asteroidVelocity, Vector2 asteroidDirection);

// Program main entry point
int main(void)
{
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    SetTargetFPS(60);
    
    // Variables
    
    bool gameOver = false;
    
    // Timer
    float gameTimer = 0.0f;
    float speedIncreaseInterval = 10.0f;
    
    float asteroidSpawnTimer = 0.0f;
    float asteroidSpawnInterval = 1.0f;
    
    Ship ship;
    Bullet bullet[MAX_BULLETS];
    Asteroid largeAsteroid[MAX_LARGE_ASTEROIDS];
    Asteroid smallAsteroid[MAX_SMALL_ASTEROIDS];
    
    // Initialize ship
    ship.pos = { (float) screenWidth / 2, (float) screenHeight / 2 };
    ship.velocity = { 0.0f, 0.0f };
    ship.thrust = 0.1f;
    ship.friction = 0.99f; // 1.0 for no friction, lower = more friction
    ship.color = DARKBLUE;
    ship.rotation = 0.0f;
    ship.size = 15.0f;
    
    // Initialize bullets
    for(int i = 0;
        i < MAX_BULLETS;
        i++)
    {
        bullet[i].active = false;
    }
    
    // Initialize asteroids
    for(int i = 0;
        i < MAX_LARGE_ASTEROIDS;
        i++)
    {
        largeAsteroid[i].active = false;
    }
    
    // Initialize small asteroids
    for(int i = 0;
        i < MAX_SMALL_ASTEROIDS;
        i++)
    {
        smallAsteroid[i].active = false;
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
            if(IsKeyDown(KEY_RIGHT)) ship.rotation += 0.05f;
            if(IsKeyDown(KEY_LEFT)) ship.rotation -= 0.05f;
            if(IsKeyDown(KEY_UP))
            {
                ship.velocity.x += sinf(ship.rotation) * ship.thrust;
                ship.velocity.y -= cosf(ship.rotation) * ship.thrust;
            }
            if(IsKeyDown(KEY_DOWN))
            {
                ship.velocity.x -= sinf(ship.rotation) * ship.thrust;
                ship.velocity.y += cosf(ship.rotation) * ship.thrust;
            }
            if(IsKeyPressed(KEY_SPACE))
            {
                for(int i = 0;
                    i < MAX_BULLETS;
                    i++)
                {
                    if(!bullet[i].active)
                    {
                        bullet[i].active = true;
                        bullet[i].pos = ship.pos;
                        bullet[i].velocity.x = sinf(ship.rotation) * bulletSpeed;
                        bullet[i].velocity.y = -cosf(ship.rotation) * bulletSpeed;
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
            ship.pos.x += ship.velocity.x;
            ship.pos.y += ship.velocity.y;
            
            ship.velocity.x *= ship.friction;
            ship.velocity.y *= ship.friction;
            
            // Update active bullets
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(bullet[i].active)
                {
                    bullet[i].pos.x += bullet[i].velocity.x;
                    bullet[i].pos.y += bullet[i].velocity.y;
                    
                    // Deactive if off screen
                    if(bullet[i].pos.x < 0 || bullet[i].pos.x > screenWidth ||
                       bullet[i].pos.y < 0 || bullet[i].pos.y > screenHeight)
                    {
                        bullet[i].active = false;
                    }
                }
            }
            
            asteroidSpawnTimer += GetFrameTime();
            
            if(asteroidSpawnTimer >= asteroidSpawnInterval)
            {
                // Spawn asteroids
                for(int i = 0;
                    i < MAX_LARGE_ASTEROIDS;
                    i++)
                {
                    if(!largeAsteroid[i].active)
                    {
                        // Get random angle and scale for asteroid
                        float angle = GetRandomValue(0, 360) * DEG2RAD;
                        float spawnRadius = screenRadius + spawnMargin;
                        
                        largeAsteroid[i].pos.x = screenWidth / 2.0f + cosf(angle) * spawnRadius;
                        largeAsteroid[i].pos.y = screenHeight / 2.0f + sinf(angle) * spawnRadius;
                        
                        largeAsteroid[i].size = GetRandomValue(20, 80);
                        largeAsteroid[i].active = true;
                        largeAsteroid[i].direction = Vector2Normalize(Vector2Subtract(asteroidTarget, largeAsteroid[i].pos));
                        largeAsteroid[i].velocity = Vector2Scale(largeAsteroid[i].direction, asteroidSpeed * asteroidSpeedMultiplier);
                        
                        // Only spawn one asteroid per interval
                        asteroidSpawnTimer = 0.0f;
                        break;
                    }
                }
            }
            
            // Update spawned asteroids
            for(int i = 0;
                i < MAX_LARGE_ASTEROIDS;
                i++)
            {
                if(largeAsteroid[i].active)
                {
                    largeAsteroid[i].pos.x += largeAsteroid[i].velocity.x;
                    largeAsteroid[i].pos.y += largeAsteroid[i].velocity.y;
                    
                    // if asteroid goes off screen then de-spawn
                    float margin = 200.0f;
                    if(largeAsteroid[i].pos.x < -margin || largeAsteroid[i].pos.x > screenWidth + margin ||
                       largeAsteroid[i].pos.y < -margin || largeAsteroid[i].pos.y > screenHeight + margin)
                    {
                        largeAsteroid[i].active = false;
                    }
                }
            }
            
            // Update spawned small asteroids
            for(int i = 0;
                i < MAX_SMALL_ASTEROIDS;
                i++)
            {
                if(smallAsteroid[i].active)
                {
                    smallAsteroid[i].pos.x += smallAsteroid[i].velocity.x;
                    smallAsteroid[i].pos.y += smallAsteroid[i].velocity.y;
                    
                    // if asteroid goes off screen then de-spawn
                    float margin = 175.00;
                    if(smallAsteroid[i].pos.x < -margin || smallAsteroid[i].pos.x > screenWidth + margin ||
                       smallAsteroid[i].pos.y < -margin || smallAsteroid[i].pos.y > screenHeight + margin)
                    {
                        smallAsteroid[i].active = false;
                    }
                } 
            }
            
            // Check for bullet asteroid collisions
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(bullet[i].active)
                {
                    for(int j = 0;
                        j < MAX_LARGE_ASTEROIDS;
                        j++)
                    {
                        if(largeAsteroid[j].active)
                        {
                            float distance = Vector2Distance(bullet[i].pos, largeAsteroid[j].pos);
                            
                            if(distance < (largeAsteroid[j].size + bulletRadius))
                            {
                                bullet[i].active = false;
                                largeAsteroid[j].active = false;
                                
                                // Spawn 2 small asteroids
                                spawnSmallAsteroid(smallAsteroid, largeAsteroid[j].pos, largeAsteroid[j].velocity, largeAsteroid[j].direction);
                                spawnSmallAsteroid(smallAsteroid, largeAsteroid[j].pos, largeAsteroid[j].velocity, largeAsteroid[j].direction);
                            }
                        }
                    }
                    for(int k = 0;
                        k < MAX_SMALL_ASTEROIDS;
                        k++)
                    {
                        if(smallAsteroid[k].active)
                        {
                            float distance = Vector2Distance(bullet[i].pos, smallAsteroid[k].pos);
                            
                            if(distance < (smallAsteroid[k].size + bulletRadius))
                            {
                                bullet[i].active = false;
                                smallAsteroid[k].active = false;
                            }
                        }
                    }
                }
            }
            
            // Check for asteroid player collisions
            if(!gameOver)
            {
                for(int i = 0;
                    i < MAX_LARGE_ASTEROIDS;
                    i++)
                {
                    if(largeAsteroid[i].active)
                    {
                        float distance = Vector2Distance(largeAsteroid[i].pos, ship.pos);
                        if(distance < (largeAsteroid[i].size + ship.size))
                        {
                            gameOver = true;
                            break;
                        }
                    }
                }
                for(int j = 0;
                    j < MAX_SMALL_ASTEROIDS;
                    j++)
                {
                    if(smallAsteroid[j].active)
                    {
                        float distance = Vector2Distance(smallAsteroid[j].pos, ship.pos);
                        if(distance < (smallAsteroid[j].size + ship.size))
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
            if(ship.pos.x < 0 || ship.pos.x > screenWidth ||
               ship.pos.y < 0 || ship.pos.y > screenHeight)
            {
                // Wrap horizontally
                if(ship.pos.x < 0) ship.pos.x = screenWidth;
                if(ship.pos.x > screenWidth) ship.pos.x = 0;
                
                // Wrap veritcally
                if(ship.pos.y < 0) ship.pos.y = screenHeight;
                if(ship.pos.y > screenHeight) ship.pos.y = 0;
            }
        }
        
        //-----------------------------------------------------------------------------------------
        // Draw
        //-----------------------------------------------------------------------------------------
        BeginDrawing();
        
        {
            ClearBackground(RAYWHITE);
            
            Vector2 v1 = {
                ship.pos.x + sinf(ship.rotation) * ship.size,
                ship.pos.y - cosf(ship.rotation) * ship.size
            };
            
            Vector2 v2 = {
                ship.pos.x + sinf(ship.rotation + 2.4f) * ship.size,
                ship.pos.y - cosf(ship.rotation + 2.4f) * ship.size
            };
            
            Vector2 v3 = {
                ship.pos.x + sinf(ship.rotation - 2.4f) * ship.size,
                ship.pos.y - cosf(ship.rotation - 2.4f) * ship.size
            };
            
            DrawTriangle(v1, v3, v2, ship.color);
            DrawTriangleLines(v1, v3, v2, BLACK);
            
            // Draw bullets
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(bullet[i].active)
                {
                    DrawCircleV(bullet[i].pos, 3.0f, RED);
                }
            }
            
            // Draw asteroids
            for(int i = 0;
                i < MAX_LARGE_ASTEROIDS;
                i++)
            {
                if(largeAsteroid[i].active)
                {
                    DrawCircleV(largeAsteroid[i].pos, largeAsteroid[i].size, GRAY);
                }
            }
            
            // Draw small asteroids
            for(int i = 0;
                i < MAX_SMALL_ASTEROIDS;
                i++)
            {
                if(smallAsteroid[i].active)
                {
                    DrawCircleV(smallAsteroid[i].pos, smallAsteroid[i].size, GRAY);
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

void
spawnSmallAsteroid(Asteroid *smallAsteroid, Vector2 asteroidPos, Vector2 asteroidVelocity, Vector2 asteroidDirection)
{
    for(int i = 0;
        i < MAX_SMALL_ASTEROIDS;
        i++)
    {
        if(!smallAsteroid[i].active)
        {
            smallAsteroid[i].pos.x = asteroidPos.x;
            smallAsteroid[i].pos.y = asteroidPos.y;
            
            smallAsteroid[i].size = GetRandomValue(5, 10);
            smallAsteroid[i].active = true;
            
            // Add some spread
            float spread = GetRandomValue(-40, 40) * DEG2RAD;
            
            smallAsteroid[i].direction.x = asteroidDirection.x * cosf(spread) - asteroidDirection.y * sinf(spread);
            smallAsteroid[i].direction.y = asteroidDirection.x * sinf(spread) + asteroidDirection.y * cosf(spread);
            
            smallAsteroid[i].velocity = Vector2Scale(smallAsteroid[i].direction, asteroidSpeed);
            
            // Only spawn one small asteroid
            break;
        }
    }
}