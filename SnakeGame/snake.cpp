#include "snake.h"

#include <stdlib.h>

// Defines the game area where the snake and food are rendered.
#define GAME_X 150
#define GAME_Y 100
#define GAME_W 700
#define GAME_H 500

// Initializes the snake with its starting position,
// length, and movement direction.
void snake_init(Snake* snake)
{
    snake->length = 3;
    snake->dirX = 1;
    snake->dirY = 0;

    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = 10 - i;
        snake->body[i].y = 10;
    }
}

// Changes the snake's movement direction.
// Opposite direction changes are ignored.
void snake_change_dir(
    Snake* snake,
    int dirX,
    int dirY
)
{
    if (snake->dirX == -dirX && snake->dirY == -dirY) {
        return;
    }

    snake->dirX = dirX;
    snake->dirY = dirY;
}

// Updates the snake position by moving every body
// segment and advancing the head.
void snake_update(Snake* snake)
{
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0].x += snake->dirX;
    snake->body[0].y += snake->dirY;
}

// Draws the snake on the screen.
void snake_render(
    Snake* snake,
    SDL_Renderer* renderer
)
{
    for (int i = 0; i < snake->length; i++) {
        SDL_FRect rect;

        rect.x = GAME_X + snake->body[i].x * GRID_SIZE;
        rect.y = GAME_Y + snake->body[i].y * GRID_SIZE;
        rect.w = GRID_SIZE - 2;
        rect.h = GRID_SIZE - 2; 

        if (i == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 0, 180, 80, 255);
        }

        SDL_RenderFillRect(renderer, &rect);
    }
}

// Spawns a food item at a random position inside
// the game area.
/*void food_spawn(Food* food)
{
    int maxX = GAME_W / GRID_SIZE;
    int maxY = GAME_H / GRID_SIZE;

    food->pos.x = rand() % maxX;
    food->pos.y = rand() % maxY;

    food->type = (rand() % 5 == 0) ? 1 : 0;
}

// Spawns a special food item.
void special_food_spawn(Food* food)
{
    int maxX = GAME_W / GRID_SIZE;
    int maxY = GAME_H / GRID_SIZE;

    food->pos.x = rand() % maxX;
    food->pos.y = rand() % maxY;

    // 20% chance of spawning special food.
    food->type = (rand() % 5 == 0) ? 1 : 0;
}

*/

void food_spawn(
    Food* food,
    Snake* snake
)
{
    int maxX = GAME_W / GRID_SIZE;
    int maxY = GAME_H / GRID_SIZE;

    bool valid = false;

    while (!valid)
    {
        valid = true;

        food->pos.x = rand() % maxX;
        food->pos.y = rand() % maxY;

        for (int i = 0; i < snake->length; i++)
        {
            if (food->pos.x == snake->body[i].x &&
                food->pos.y == snake->body[i].y)
            {
                valid = false;
                break;
            }
        }
    }

    int r = rand() % 10;

    if (r == 0) food->type = 1;   // poison
    else if (r < 3) food->type = -1; // special
    else food->type = 0;           // normal
}



// Draws the food on the screen.
void food_render(
    Food* food,
    SDL_Renderer* renderer
)
{
    SDL_FRect rect;

    rect.x = GAME_X + food->pos.x * GRID_SIZE;
    rect.y = GAME_Y + food->pos.y * GRID_SIZE;
    rect.w = GRID_SIZE;
    rect.h = GRID_SIZE;

    if (food->type == 1) {
        // Special food (Gold)
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    }
    else if (food->type == -1) {
        // Poisonous food (Purple / Dark Violet)
        SDL_SetRenderDrawColor(renderer, 148, 0, 211, 255);
    }
    else {
        // Normal food (Red)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    SDL_FRect drawRect = {
        rect.x + 2,
        rect.y + 2,
        rect.w - 4,
        rect.h - 4
    };

    SDL_RenderFillRect(renderer, &drawRect);
}