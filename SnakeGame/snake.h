#ifndef SNAKE_H
#define SNAKE_H

#include <SDL3/SDL.h>

#define MAX_SNAKE 100
#define GRID_SIZE 20

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[MAX_SNAKE];
    int length;
    int dirX;
    int dirY;
} Snake;

typedef struct {
    Point pos;
    int type; // 0 = normal, 1 = special
} Food;

// snake
void snake_init(Snake* s);
void snake_update(Snake* s);
void snake_render(Snake* s, SDL_Renderer* renderer);
void snake_change_dir(Snake* s, int x, int y);

// food
void food_spawn(Food* f);
void food_render(Food* f, SDL_Renderer* renderer);

#endif