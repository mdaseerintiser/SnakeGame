#include "snake.h"
#include <stdlib.h>

// GAME AREA
#define GAME_X 150
#define GAME_Y 100
#define GAME_W 500
#define GAME_H 400

void snake_init(Snake* s)
{
    s->length = 3;
    s->dirX = 1;
    s->dirY = 0;

    for (int i = 0; i < s->length; i++)
    {
        s->body[i].x = 10 - i;
        s->body[i].y = 10;
    }
}

void snake_change_dir(Snake* s, int x, int y)
{
    if (s->dirX == -x && s->dirY == -y)
        return;

    s->dirX = x;
    s->dirY = y;
}

void snake_update(Snake* s)
{
    for (int i = s->length - 1; i > 0; i--)
    {
        s->body[i] = s->body[i - 1];
    }

    s->body[0].x += s->dirX;
    s->body[0].y += s->dirY;
}

void snake_render(Snake* s, SDL_Renderer* renderer)
{
    for (int i = 0; i < s->length; i++)
    {
        SDL_FRect r;

        r.x = GAME_X + s->body[i].x * GRID_SIZE;
        r.y = GAME_Y + s->body[i].y * GRID_SIZE;

        r.w = GRID_SIZE - 2;
        r.h = GRID_SIZE - 2;

        if (i == 0)
            SDL_SetRenderDrawColor(renderer, 0, 255, 120, 255);
        else
            SDL_SetRenderDrawColor(renderer, 0, 180, 80, 255);

        SDL_RenderFillRect(renderer, &r);
    }
}

// 🍎 FOOD SPAWN (100% SAFE INSIDE BOX)
void food_spawn(Food* f)
{
    int maxX = GAME_W / GRID_SIZE;
    int maxY = GAME_H / GRID_SIZE;

    f->pos.x = rand() % maxX;
    f->pos.y = rand() % maxY;

    f->type = (rand() % 5 == 0) ? 1 : 0;
}




void special_food_spawn(Food* f)
{
    int maxX = GAME_W / GRID_SIZE;
    int maxY = GAME_H / GRID_SIZE;

    f->pos.x = rand() % maxX;
    f->pos.y = rand() % maxY;

    // 20% chance special food
    f->type = (rand() % 5 == 0) ? 1 : 0;
}

// 🍎 FOOD RENDER (small rounded look, still square-safe)
void food_render(Food* f, SDL_Renderer* renderer)
{
    SDL_FRect r;

    r.x = GAME_X + f->pos.x * GRID_SIZE;
    r.y = GAME_Y + f->pos.y * GRID_SIZE;

    r.w = GRID_SIZE;
    r.h = GRID_SIZE;

    if (f->type == 1)
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // special
    else
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);   // normal

    // small inset (looks slightly round but safe)
    SDL_FRect draw =
    {
        r.x + 2,
        r.y + 2,
        r.w - 4,
        r.h - 4
    };




    SDL_RenderFillRect(renderer, &draw);
}