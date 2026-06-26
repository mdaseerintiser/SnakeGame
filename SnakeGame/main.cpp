
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "snake.h"

#define SCREEN_W 800
#define SCREEN_H 600

// Defines the game area.
#define GAME_X 150
#define GAME_Y 100
#define GAME_W 500
#define GAME_H 400

// Renders text at the specified screen position.
void draw_text(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    int x,
    int y
)
{
    SDL_Color white = {
        255,
        255,
        255,
        255
    };

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            font,
            text,
            0,
            white
        );

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer,
            surface
        );

    SDL_DestroySurface(surface);

    float width;
    float height;

    SDL_GetTextureSize(
        texture,
        &width,
        &height
    );

    SDL_FRect dst = {
        x,
        y,
        width,
        height
    };

    SDL_RenderTexture(
        renderer,
        texture,
        NULL,
        &dst
    );

    SDL_DestroyTexture(texture);
}

// Entry point of the Snake game.
int main(
    int argc,
    char* argv[]
)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window =
        SDL_CreateWindow(
            "Snake Game",
            SCREEN_W,
            SCREEN_H,
            0
        );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    TTF_Font* font =
        TTF_OpenFont(
            "font.ttf",
            24.0f
        );

    if (!font) {
        printf(
            "Font error: %s\n",
            SDL_GetError()
        );

        return 1;
    }

    Snake snake;
    Food food;

    snake_init(&snake);
    food_spawn(&food);

    int score = 0;

    time_t startTime = time(NULL);
    time_t gameOverTime = 0;

    bool running = true;
    bool gameOver = false;

    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (!gameOver &&
                event.type == SDL_EVENT_KEY_DOWN) {

                switch (event.key.key) {

                case SDLK_UP:
                    snake_change_dir(
                        &snake,
                        0,
                        -1
                    );
                    break;

                case SDLK_DOWN:
                    snake_change_dir(
                        &snake,
                        0,
                        1
                    );
                    break;

                case SDLK_LEFT:
                    snake_change_dir(
                        &snake,
                        -1,
                        0
                    );
                    break;

                case SDLK_RIGHT:
                    snake_change_dir(
                        &snake,
                        1,
                        0
                    );
                    break;
                }
            }
        }

        // Update the game state.
        if (!gameOver) {

            snake_update(&snake);

            // Check if the snake has eaten the food.
            if (snake.body[0].x == food.pos.x &&
                snake.body[0].y == food.pos.y) {

                if (snake.length < MAX_SNAKE) {
                    snake.body[snake.length] =
                        snake.body[snake.length - 1];

                    snake.length++;
                }

                if (food.type == 1) {
                    score += 50;
                }
                else {
                    score += 10;
                }

                food_spawn(&food);
            }

            // Check for wall collision.
            int maxX = GAME_W / GRID_SIZE;
            int maxY = GAME_H / GRID_SIZE;

            if (snake.body[0].x < 0 ||
                snake.body[0].y < 0 ||
                snake.body[0].x >= maxX ||
                snake.body[0].y >= maxY) {

                gameOver = true;
                gameOverTime = time(NULL);
            }

            // Check for collision with the snake body.
            for (int i = 1; i < snake.length; i++) {

                if (snake.body[0].x == snake.body[i].x &&
                    snake.body[0].y == snake.body[i].y) {

                    gameOver = true;
                    gameOverTime = time(NULL);
                }
            }
        }

        // Calculate the elapsed game time.
        int elapsed;

        if (!gameOver) {
            elapsed = (int)(time(NULL) - startTime);
        }
        else {
            elapsed = (int)(gameOverTime - startTime);
        }

        int min = elapsed / 60;
        int sec = elapsed % 60;

        char timeText[64];

        snprintf(
            timeText,
            sizeof(timeText),
            "Time: %02d:%02d",
            min,
            sec
        );

        char scoreText[64];

        snprintf(
            scoreText,
            sizeof(scoreText),
            "Score: %d",
            score
        );



        // Render the current frame.
        SDL_SetRenderDrawColor(
            renderer,
            25,
            25,
            25,
            255
        );

        SDL_RenderClear(renderer);

        draw_text(
            renderer,
            font,
            scoreText,
            20,
            20
        );

        draw_text(
            renderer,
            font,
            timeText,
            600,
            20
        );

        // Draw the game area border.
        SDL_FRect border = {
            GAME_X,
            GAME_Y,
            GAME_W,
            GAME_H
        };

        SDL_SetRenderDrawColor(
            renderer,
            200,
            200,
            200,
            255
        );

        SDL_RenderRect(
            renderer,
            &border
        );

        // Draw game objects.
        if (!gameOver) {

            food_render(
                &food,
                renderer
            );

            snake_render(
                &snake,
                renderer
            );
        }
        else {

            draw_text(
                renderer,
                font,
                "GAME OVER",
                320,
                250
            );

            char finalText[64];

            snprintf(
                finalText,
                sizeof(finalText),
                "Final Score: %d",
                score
            );

            draw_text(
                renderer,
                font,
                finalText,
                300,
                300
            );
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(120);
    }

    // Release all allocated resources.
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    TTF_Quit();

    SDL_Quit();

    return 0;
}