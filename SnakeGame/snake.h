#ifndef SNAKE_H
#define SNAKE_H

#include <SDL3/SDL.h>

#define MAX_SNAKE 100
#define GRID_SIZE 20
#define MAX_OBSTACLES 20


// Represents a position in the game grid.
typedef struct {
    int x;
    int y;
} Point;



typedef struct {
    Point pos;
} Obstacle;


typedef struct {
    Point pos;
} Portal;

// Represents the snake, including its body, length,
// and current movement direction.
typedef struct {
    Point body[MAX_SNAKE];
    int length;
    int dirX;
    int dirY;
} Snake;

// Represents a food item that can be collected by
// the snake. Type indicates the food category.
typedef struct {
    Point pos;
    int type;   // 0 = Normal, 1 = Special , -1 = Poisonous 
} Food;

// Initializes the snake with its starting position,
// length, and movement direction.
void snake_init(Snake* snake);

// Updates the snake's position based on its
// current movement direction.
void snake_update(Snake* snake);

// Draws the snake on the screen.
void snake_render(Snake* snake, SDL_Renderer* renderer);

// Changes the snake's movement direction.
void snake_change_dir(
    Snake* snake,
    int dirX,
    int dirY
);

// Spawns a new food item at a valid position.
void food_spawn(Food* food,Snake* snake);

// Draws the food item on the screen.
void food_render(
    Food* food,
    SDL_Renderer* renderer
);

#endif