#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "snake.h"

#define SCREEN_W 1000
#define SCREEN_H 1000

#define GAME_X 150
#define GAME_Y 100
#define GAME_W 700
#define GAME_H 500

Portal portalA;
Portal portalB;
bool portalActive = true;


// ---------------- TEXT RENDER ----------------
void draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y)
{
    SDL_Color white = { 255, 255, 255, 255 };

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    float w, h;
    SDL_GetTextureSize(texture, &w, &h);

    SDL_FRect dst = { x, y, w, h };

    SDL_RenderTexture(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

// ---------------- OBSTACLES ----------------

Obstacle obstacles[MAX_OBSTACLES];
int obstacleCount = 10;

// spawn obstacles
void spawn_obstacles(Obstacle obstacles[], int count, Snake* snake)
{
    int maxX = GAME_W / GRID_SIZE;
    int maxY = GAME_H / GRID_SIZE;

    for (int i = 0; i < count; i++)
    {
        bool valid = false;

        while (!valid)
        {
            valid = true;

            obstacles[i].pos.x = rand() % maxX;
            obstacles[i].pos.y = rand() % maxY;

            for (int j = 0; j < snake->length; j++)
            {
                if (obstacles[i].pos.x == snake->body[j].x &&
                    obstacles[i].pos.y == snake->body[j].y)
                {
                    valid = false;
                    break;
                }
            }
        }
    }
}

// ---------------- MAIN ----------------
int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Snake Game", SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    TTF_Font* font = TTF_OpenFont("font.ttf", 24.0f);

    if (!font)
    {
        printf("Font error\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    Snake snake;
    Food food;

    snake_init(&snake);
    food_spawn(&food, &snake);
    spawn_obstacles(obstacles, obstacleCount, &snake);

    int score = 0;

    bool running = true;
    bool gameOver = false;
    bool paused = false;

    time_t startTime = time(NULL);
    time_t gameOverTime = 0;
    time_t foodSpawnTime = time(NULL);

    SDL_Event event;

    // ---------------- GAME LOOP ----------------
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                // pause toggle
                if (event.key.scancode == SDL_SCANCODE_P)
                    paused = !paused;

                // movement
                if (!gameOver && !paused)
                {
                    switch (event.key.key)
                    {
                    case SDLK_UP: snake_change_dir(&snake, 0, -1); break;
                    case SDLK_DOWN: snake_change_dir(&snake, 0, 1); break;
                    case SDLK_LEFT: snake_change_dir(&snake, -1, 0); break;
                    case SDLK_RIGHT: snake_change_dir(&snake, 1, 0); break;
                    }
                }
            }
        }

        // ---------------- UPDATE ----------------
        if (!gameOver && !paused)
        {
            snake_update(&snake);

            // food eat
            if (snake.body[0].x == food.pos.x &&
                snake.body[0].y == food.pos.y)
            {
                if (food.type == -1)
                {
                    if (snake.length > 3)
                        snake.length--;

                    score -= 20;
                }
                else
                {
                    if (snake.length < MAX_SNAKE)
                    {
                        snake.body[snake.length] =
                            snake.body[snake.length - 1];
                        snake.length++;
                    }

                    if (food.type == 1)
                        score += 50;
                    else
                        score += 10;
                }

                food_spawn(&food, &snake);
                foodSpawnTime = time(NULL);
            }

            // poison timeout (10 sec)
            if (food.type == -1 &&
                time(NULL) - foodSpawnTime >= 10)
            {
                food_spawn(&food, &snake);
                foodSpawnTime = time(NULL);
            }

            // wall collision
            int maxX = GAME_W / GRID_SIZE;
            int maxY = GAME_H / GRID_SIZE;

            if (snake.body[0].x < 0 || snake.body[0].y < 0 ||
                snake.body[0].x >= maxX || snake.body[0].y >= maxY)
            {
                gameOver = true;
                gameOverTime = time(NULL);
            }

            // self collision
            for (int i = 1; i < snake.length; i++)
            {
                if (snake.body[0].x == snake.body[i].x &&
                    snake.body[0].y == snake.body[i].y)
                {
                    gameOver = true;
                    gameOverTime = time(NULL);
                }
            }

            // obstacle collision
            for (int i = 0; i < obstacleCount; i++)
            {
                if (snake.body[0].x == obstacles[i].pos.x &&
                    snake.body[0].y == obstacles[i].pos.y)
                {
                    gameOver = true;
                    gameOverTime = time(NULL);
                }
            }
        }

        // ---------------- TIME ----------------
        int elapsed = (!gameOver)
            ? (int)(time(NULL) - startTime)
            : (int)(gameOverTime - startTime);

        int min = elapsed / 60;
        int sec = elapsed % 60;

        char timeText[64];
        snprintf(timeText, sizeof(timeText), "Time: %02d:%02d", min, sec);

        char scoreText[64];
        snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

        // ---------------- RENDER ----------------
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);

        draw_text(renderer, font, scoreText, 20, 20);
        draw_text(renderer, font, timeText, 600, 20);

        // border
        SDL_FRect border = { GAME_X, GAME_Y, GAME_W, GAME_H };
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderRect(renderer, &border);

        if (!gameOver)
        {
            food_render(&food, renderer);
            snake_render(&snake, renderer);

            // obstacles
            for (int i = 0; i < obstacleCount; i++)
            {
                SDL_FRect r = {
                    GAME_X + obstacles[i].pos.x * GRID_SIZE,
                    GAME_Y + obstacles[i].pos.y * GRID_SIZE,
                    GRID_SIZE - 2,
                    GRID_SIZE - 2
                };

                SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
                SDL_RenderFillRect(renderer, &r);
            }

            if (paused)
            {
                draw_text(renderer, font, "PAUSED (P to resume)", 350, 250);
            }
        }
        else
        {
            draw_text(renderer, font, "GAME OVER", 380, 250);

            char finalText[64];
            snprintf(finalText, sizeof(finalText), "Final Score: %d", score);

            draw_text(renderer, font, finalText, 350, 300);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(120);
    }

    // ---------------- CLEANUP ----------------
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}