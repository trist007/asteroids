#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <stdint.h>
#include "raylib.h"
#include "raymath.h"

#define MAX_BULLETS 20
#define MAX_LARGE_ASTEROIDS 8
#define MAX_SMALL_ASTEROIDS 12

// Screen resolution
int screenWidth = 1024;
int screenHeight = 768;

// Getting radius of screen windows for asteroid spawn
float screenRadius = sqrtf((screenWidth * screenWidth) + (screenHeight * screenHeight)) / 2;

// Spawn margin so asteroid will not spawn within the screen
float spawnMargin = 50.0f;

// Helper macros
#define Assert(expr) if(!(expr)) { *(int *)0 = 0; }
#define MEGABYTES(num) ((num) * 1024ULL * 1024ULL)
#define arena_push(arena, type) (type *)arena_alloc(arena, sizeof(type))

typedef struct
{
    //uint8_t *base;
    unsigned char *base;
    size_t size;
    size_t used;
} Arena;

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

typedef struct
{
    // Assets
    Ship ship;
    Bullet bullet[MAX_BULLETS];
    Asteroid largeAsteroid[MAX_LARGE_ASTEROIDS];
    Asteroid smallAsteroid[MAX_SMALL_ASTEROIDS];
    
    // Asteroid attributes
    float asteroidSpeed;
    Vector2 asteroidTarget;
    
    // Implement difficulty where every 10 seconds 0.2f gets added
    float asteroidSpeedMultiplier;
    
    // size of bullet
    float bulletRadius;
    float bulletSpeed;
    
    // Variables
    bool gameOver;
    
    // Timer
    float gameTimer;
    float speedIncreaseInterval;
    
    float asteroidSpawnTimer;
    float asteroidSpawnInterval;
    
} GameState;

// Forward declarations / Function prototypes
GameState *initializeGame(Arena *arena);
void spawnSmallAsteroid(GameState *gs, Vector2 asteroidPos, Vector2 asteroidVelocity, Vector2 asteroidDirection);
void *arena_alloc(Arena *a, size_t bytes);

// Program main entry point
int main(void)
{
    InitWindow(screenWidth, screenHeight, "asteroids");
    
    SetTargetFPS(60);
    
    void *memory = VirtualAlloc(NULL, MEGABYTES(64), MEM_COMMIT | MEM_RESERVE ,PAGE_READWRITE);
    Arena arena;
    arena.base = (unsigned char *)memory;
    arena.size = MEGABYTES(64);
    arena.used = 0;
    
    // Initialize game_state
    GameState *gs = initializeGame(&arena);
    
    // Main game loop
    while(!WindowShouldClose())
    {
        //-----------------------------------------------------------------------------------------
        // Update
        //-----------------------------------------------------------------------------------------
        if(!gs->gameOver)
        {
            if(IsKeyPressed(KEY_H))
            {
                if(IsCursorHidden()) ShowCursor();
                else HideCursor();
            }
            
            // Control ship with keyboard
            if(IsKeyDown(KEY_RIGHT)) gs->ship.rotation += 0.05f;
            if(IsKeyDown(KEY_LEFT)) gs->ship.rotation -= 0.05f;
            if(IsKeyDown(KEY_UP))
            {
                gs->ship.velocity.x += sinf(gs->ship.rotation) * gs->ship.thrust;
                gs->ship.velocity.y -= cosf(gs->ship.rotation) * gs->ship.thrust;
            }
            if(IsKeyDown(KEY_DOWN))
            {
                gs->ship.velocity.x -= sinf(gs->ship.rotation) * gs->ship.thrust;
                gs->ship.velocity.y += cosf(gs->ship.rotation) * gs->ship.thrust;
            }
            if(IsKeyPressed(KEY_SPACE))
            {
                for(int i = 0;
                    i < MAX_BULLETS;
                    i++)
                {
                    if(!gs->bullet[i].active)
                    {
                        gs->bullet[i].active = true;
                        gs->bullet[i].pos = gs->ship.pos;
                        gs->bullet[i].velocity.x = sinf(gs->ship.rotation) * gs->bulletSpeed;
                        gs->bullet[i].velocity.y = -cosf(gs->ship.rotation) * gs->bulletSpeed;
                        break;
                    }
                }
            }
            
            // Adjust asteroid speed
            gs->gameTimer += GetFrameTime();
            if(gs->gameTimer >= gs->speedIncreaseInterval)
            {
                gs->asteroidSpeedMultiplier += 0.5f;
                gs->gameTimer = 0.0f;
            }
            
            // Apply velocity and friction to shipPosition
            gs->ship.pos.x += gs->ship.velocity.x;
            gs->ship.pos.y += gs->ship.velocity.y;
            
            gs->ship.velocity.x *= gs->ship.friction;
            gs->ship.velocity.y *= gs->ship.friction;
            
            // Update active bullets
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(gs->bullet[i].active)
                {
                    gs->bullet[i].pos.x += gs->bullet[i].velocity.x;
                    gs->bullet[i].pos.y += gs->bullet[i].velocity.y;
                    
                    // Deactive if off screen
                    if(gs->bullet[i].pos.x < 0 || gs->bullet[i].pos.x > screenWidth ||
                       gs->bullet[i].pos.y < 0 || gs->bullet[i].pos.y > screenHeight)
                    {
                        gs->bullet[i].active = false;
                    }
                }
            }
            
            gs->asteroidSpawnTimer += GetFrameTime();
            
            if(gs->asteroidSpawnTimer >= gs->asteroidSpawnInterval)
            {
                // Spawn asteroids
                for(int i = 0;
                    i < MAX_LARGE_ASTEROIDS;
                    i++)
                {
                    if(!gs->largeAsteroid[i].active)
                    {
                        // Get random angle and scale for asteroid
                        float angle = GetRandomValue(0, 360) * DEG2RAD;
                        float spawnRadius = screenRadius + spawnMargin;
                        
                        gs->largeAsteroid[i].pos.x = screenWidth / 2.0f + cosf(angle) * spawnRadius;
                        gs->largeAsteroid[i].pos.y = screenHeight / 2.0f + sinf(angle) * spawnRadius;
                        
                        gs->largeAsteroid[i].size = GetRandomValue(20, 80);
                        gs->largeAsteroid[i].active = true;
                        gs->largeAsteroid[i].direction = Vector2Normalize(Vector2Subtract(gs->asteroidTarget, gs->largeAsteroid[i].pos));
                        gs->largeAsteroid[i].velocity = Vector2Scale(gs->largeAsteroid[i].direction, gs->asteroidSpeed * gs->asteroidSpeedMultiplier);
                        
                        // Only spawn one asteroid per interval
                        gs->asteroidSpawnTimer = 0.0f;
                        break;
                    }
                }
            }
            
            // Update spawned asteroids
            for(int i = 0;
                i < MAX_LARGE_ASTEROIDS;
                i++)
            {
                if(gs->largeAsteroid[i].active)
                {
                    gs->largeAsteroid[i].pos.x += gs->largeAsteroid[i].velocity.x;
                    gs->largeAsteroid[i].pos.y += gs->largeAsteroid[i].velocity.y;
                    
                    // if asteroid goes off screen then de-spawn
                    float margin = 200.0f;
                    if(gs->largeAsteroid[i].pos.x < -margin || gs->largeAsteroid[i].pos.x > screenWidth + margin ||
                       gs->largeAsteroid[i].pos.y < -margin || gs->largeAsteroid[i].pos.y > screenHeight + margin)
                    {
                        gs->largeAsteroid[i].active = false;
                    }
                }
            }
            
            // Update spawned small asteroids
            for(int i = 0;
                i < MAX_SMALL_ASTEROIDS;
                i++)
            {
                if(gs->smallAsteroid[i].active)
                {
                    gs->smallAsteroid[i].pos.x += gs->smallAsteroid[i].velocity.x;
                    gs->smallAsteroid[i].pos.y += gs->smallAsteroid[i].velocity.y;
                    
                    // if asteroid goes off screen then de-spawn
                    float margin = 175.00;
                    if(gs->smallAsteroid[i].pos.x < -margin || gs->smallAsteroid[i].pos.x > screenWidth + margin ||
                       gs->smallAsteroid[i].pos.y < -margin || gs->smallAsteroid[i].pos.y > screenHeight + margin)
                    {
                        gs->smallAsteroid[i].active = false;
                    }
                } 
            }
            
            // Check for bullet asteroid collisions
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(gs->bullet[i].active)
                {
                    for(int j = 0;
                        j < MAX_LARGE_ASTEROIDS;
                        j++)
                    {
                        if(gs->largeAsteroid[j].active)
                        {
                            float distance = Vector2Distance(gs->bullet[i].pos, gs->largeAsteroid[j].pos);
                            
                            if(distance < (gs->largeAsteroid[j].size + gs->bulletRadius))
                            {
                                gs->bullet[i].active = false;
                                gs->largeAsteroid[j].active = false;
                                
                                // Spawn 2 small asteroids
                                spawnSmallAsteroid(gs, gs->largeAsteroid[j].pos, gs->largeAsteroid[j].velocity, gs->largeAsteroid[j].direction);
                                spawnSmallAsteroid(gs, gs->largeAsteroid[j].pos, gs->largeAsteroid[j].velocity, gs->largeAsteroid[j].direction);
                            }
                        }
                    }
                    for(int k = 0;
                        k < MAX_SMALL_ASTEROIDS;
                        k++)
                    {
                        if(gs->smallAsteroid[k].active)
                        {
                            float distance = Vector2Distance(gs->bullet[i].pos, gs->smallAsteroid[k].pos);
                            
                            if(distance < (gs->smallAsteroid[k].size + gs->bulletRadius))
                            {
                                gs->bullet[i].active = false;
                                gs->smallAsteroid[k].active = false;
                            }
                        }
                    }
                }
            }
            
            // Check for asteroid player collisions
            if(!gs->gameOver)
            {
                for(int i = 0;
                    i < MAX_LARGE_ASTEROIDS;
                    i++)
                {
                    if(gs->largeAsteroid[i].active)
                    {
                        float distance = Vector2Distance(gs->largeAsteroid[i].pos, gs->ship.pos);
                        if(distance < (gs->largeAsteroid[i].size + gs->ship.size))
                        {
                            gs->gameOver = true;
                            break;
                        }
                    }
                }
                for(int j = 0;
                    j < MAX_SMALL_ASTEROIDS;
                    j++)
                {
                    if(gs->smallAsteroid[j].active)
                    {
                        float distance = Vector2Distance(gs->smallAsteroid[j].pos, gs->ship.pos);
                        if(distance < (gs->smallAsteroid[j].size + gs->ship.size))
                        {
                            gs->gameOver = true;
                            break;
                        }
                    }
                }
            }
            
            // Check if player ship has gone offscreen only to wrap on the opposite end
            // NOTE(trist007): if the ship moves very fast it can do multiple
            // wraps so you can use the crossedOver bool
            if(gs->ship.pos.x < 0 || gs->ship.pos.x > screenWidth ||
               gs->ship.pos.y < 0 || gs->ship.pos.y > screenHeight)
            {
                // Wrap horizontally
                if(gs->ship.pos.x < 0) gs->ship.pos.x = screenWidth;
                if(gs->ship.pos.x > screenWidth) gs->ship.pos.x = 0;
                
                // Wrap veritcally
                if(gs->ship.pos.y < 0) gs->ship.pos.y = screenHeight;
                if(gs->ship.pos.y > screenHeight) gs->ship.pos.y = 0;
            }
        }
        
        //-----------------------------------------------------------------------------------------
        // Draw
        //-----------------------------------------------------------------------------------------
        BeginDrawing();
        
        {
            ClearBackground(RAYWHITE);
            
            Vector2 v1 = {
                gs->ship.pos.x + sinf(gs->ship.rotation) * gs->ship.size,
                gs->ship.pos.y - cosf(gs->ship.rotation) * gs->ship.size
            };
            
            Vector2 v2 = {
                gs->ship.pos.x + sinf(gs->ship.rotation + 2.4f) * gs->ship.size,
                gs->ship.pos.y - cosf(gs->ship.rotation + 2.4f) * gs->ship.size
            };
            
            Vector2 v3 = {
                gs->ship.pos.x + sinf(gs->ship.rotation - 2.4f) * gs->ship.size,
                gs->ship.pos.y - cosf(gs->ship.rotation - 2.4f) * gs->ship.size
            };
            
            DrawTriangle(v1, v3, v2, gs->ship.color);
            DrawTriangleLines(v1, v3, v2, BLACK);
            
            // Draw bullets
            for(int i = 0;
                i < MAX_BULLETS;
                i++)
            {
                if(gs->bullet[i].active)
                {
                    DrawCircleV(gs->bullet[i].pos, 3.0f, RED);
                }
            }
            
            // Draw asteroids
            for(int i = 0;
                i < MAX_LARGE_ASTEROIDS;
                i++)
            {
                if(gs->largeAsteroid[i].active)
                {
                    DrawCircleV(gs->largeAsteroid[i].pos, gs->largeAsteroid[i].size, GRAY);
                }
            }
            
            // Draw small asteroids
            for(int i = 0;
                i < MAX_SMALL_ASTEROIDS;
                i++)
            {
                if(gs->smallAsteroid[i].active)
                {
                    DrawCircleV(gs->smallAsteroid[i].pos, gs->smallAsteroid[i].size, GRAY);
                }
            }
            
            if(IsCursorHidden()) DrawText("CURSOR HIDDEN", 20, 60, 20, RED);
            else DrawText("CURSOR VISIBLE", 20, 60, 20, LIME);
            
        }
        
        if(gs->gameOver)
        {
            if(IsKeyPressed(KEY_R))
            {
                gs = initializeGame(&arena);
            }
            
            int fontSize = 80;
            int fontSize2 = 50;
            const char *text = "GAME OVER";
            const char *text2 = "hit R to restart";
            int textWidth = MeasureText(text, fontSize);
            DrawText(text, screenWidth / 2 - textWidth / 2, screenHeight / 2 - fontSize / 2, fontSize, RED);
            DrawText(text2, screenWidth / 2 - textWidth / 2, (screenHeight / 2 - fontSize / 2) + 75, fontSize2, RED);
            
            
        }
        
        EndDrawing();
        //-----------------------------------------------------------------------------------------
    }
    
    // De-Initialization
    CloseWindow();
    
    return(0);
}

GameState *
initializeGame(Arena *arena)
{
    arena->used = 0;
    GameState *gs = arena_push(arena, GameState);
    
    gs->gameOver = false;
    gs->speedIncreaseInterval = 10.0f;
    gs->asteroidSpawnInterval = 1.0f;
    
    gs->asteroidSpeed = 2.0f;
    gs->asteroidTarget = { screenWidth / 2.0f, screenHeight / 2.0f };
    gs->asteroidSpeedMultiplier = 1.0f;
    gs->bulletRadius = 3.0f;
    gs->bulletSpeed = 10.0f;
    
    // Initialize ship
    gs->ship.pos = { (float)screenWidth / 2, (float)screenHeight / 2 };
    gs->ship.velocity = {};
    gs->ship.rotation = 0.0f;
    gs->ship.thrust = 0.1f;
    gs->ship.friction = 0.99f; // 1.0 for no friction, lower = more friction
    gs->ship.color = DARKBLUE;
    gs->ship.size = 15.0f;
    
    // Initialize bullets
    for(int i = 0;
        i < MAX_BULLETS;
        i++)
    {
        gs->bullet[i].active = false;
    }
    
    // Initialize asteroids
    for(int i = 0;
        i < MAX_LARGE_ASTEROIDS;
        i++)
    {
        gs->largeAsteroid[i].active = false;
    }
    
    // Initialize small asteroids
    for(int i = 0;
        i < MAX_SMALL_ASTEROIDS;
        i++)
    {
        gs->smallAsteroid[i].active = false;
    }
    
    return(gs);
}

void
spawnSmallAsteroid(GameState *gs, Vector2 asteroidPos, Vector2 asteroidVelocity, Vector2 asteroidDirection)
{
    for(int i = 0;
        i < MAX_SMALL_ASTEROIDS;
        i++)
    {
        if(!gs->smallAsteroid[i].active)
        {
            gs->smallAsteroid[i].pos.x = asteroidPos.x;
            gs->smallAsteroid[i].pos.y = asteroidPos.y;
            
            gs->smallAsteroid[i].size = GetRandomValue(5, 10);
            gs->smallAsteroid[i].active = true;
            
            // Add some spread
            float spread = GetRandomValue(-40, 40) * DEG2RAD;
            
            gs->smallAsteroid[i].direction.x = asteroidDirection.x * cosf(spread) - asteroidDirection.y * sinf(spread);
            gs->smallAsteroid[i].direction.y = asteroidDirection.x * sinf(spread) + asteroidDirection.y * cosf(spread);
            
            gs->smallAsteroid[i].velocity = Vector2Scale(gs->smallAsteroid[i].direction, gs->asteroidSpeed);
            
            // Only spawn one small asteroid
            break;
        }
    }
}

void*
arena_alloc(Arena *a, size_t bytes)
{
    Assert(a->used + bytes <= a->size);
    void *ptr = a->base + a->used;
    a->used += bytes;
    return(ptr);
}

