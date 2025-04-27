#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>

#define LENGTH 800
#define HEIGHT 600
#define MAP_ROWS 20
#define MAP_COLS 20

#define TILE_SKY 0
#define TILE_GROUND 1
#define TILE_BRICK 2

#define DEFAULT_TILE_SIZE 40

typedef struct {
  int type;   // Type of tile (sky, ground, brick, etc.)
  int width;  // Width in pixels
  int height; // Height in pixels
} MapTile;

// Global map data
MapTile gameMap[MAP_ROWS][MAP_COLS] = {0};

typedef struct Obj {
  SDL_Texture *groundTile;
  SDL_Texture *brickTile;
  // Add more tile textures as needed
} Objects;

typedef struct Player {
  SDL_Texture *player;
  int x;
  int y;
} player_t;

struct Resources {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *background;
  SDL_Rect backgroundRect;
};

bool Initialization(struct Resources *res);
void TerminateProgram(struct Resources *res);
bool LoadData(struct Resources *res, Objects *obj);
bool GenerateMap();
void RenderMap(struct Resources *res, Objects *obj);

int main() {
  struct Resources res = {
      .window = NULL,
      .renderer = NULL,
      .background = NULL,
      .backgroundRect = {0, 0, LENGTH, HEIGHT},
  };

  Objects obj = {
      .groundTile = NULL,
      .brickTile = NULL,
  };

  if (!Initialization(&res)) {
    TerminateProgram(&res);
    return 1;
  }

  if (!LoadData(&res, &obj)) {
    TerminateProgram(&res);
    return 1;
  }

  if (!GenerateMap()) {
    TerminateProgram(&res);
    return 1;
  }

  bool quit = false;
  SDL_Event event;

  while (!quit) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }

    // Clear and render background
    SDL_RenderClear(res.renderer);
    SDL_RenderCopy(res.renderer, res.background, NULL, NULL);

    // Render map tiles
    RenderMap(&res, &obj);

    // Present the rendered frame
    SDL_RenderPresent(res.renderer);
    SDL_Delay(16);
  }

  TerminateProgram(&res);
  return 0;
}

bool GenerateMap() {
  // Initialize all tiles as sky with default size
  for (int row = 0; row < MAP_ROWS; row++) {
    for (int col = 0; col < MAP_COLS; col++) {
      gameMap[row][col].type = TILE_SKY;
      gameMap[row][col].width = DEFAULT_TILE_SIZE;
      gameMap[row][col].height = DEFAULT_TILE_SIZE;
    }
  }
  for (int col = 8; col < 9; col++) {
    gameMap[9][col].type = TILE_BRICK;
    gameMap[9][col].width = DEFAULT_TILE_SIZE; // Double width
    //
    gameMap[9][col].height = DEFAULT_TILE_SIZE; // Normal height
    gameMap[9][col + 1].type = TILE_SKY;
  }

  for (int col = 7; col <= 9; col++) {
    gameMap[10][col].type = TILE_BRICK;
    gameMap[10][col].width = DEFAULT_TILE_SIZE; // Double width
    //
    gameMap[10][col].height = DEFAULT_TILE_SIZE; // Normal height
    gameMap[10][col + 1].type = TILE_SKY;
  }

  // Platform 2 - larger bricks
  for (int col = 0; col < 20; col++) {
    gameMap[13][col].type = TILE_BRICK;
    gameMap[13][col].width = DEFAULT_TILE_SIZE; // Double width
    //
    gameMap[13][col].height = DEFAULT_TILE_SIZE; // Normal height
    gameMap[13][col + 1].type = TILE_SKY;
  }

  return true;
}

void RenderMap(struct Resources *res, Objects *obj) {
  SDL_Rect tileRect;

  for (int row = 0; row < MAP_ROWS; row++) {
    for (int col = 0; col < MAP_COLS; col++) {
      MapTile *tile = &gameMap[row][col];

      if (tile->type == TILE_SKY)
        continue;

      tileRect.x = (col * DEFAULT_TILE_SIZE);
      tileRect.y = row * DEFAULT_TILE_SIZE;
      tileRect.w = tile->width;
      tileRect.h = tile->height;

      SDL_Texture *texture = NULL;
      switch (tile->type) {
      case TILE_GROUND:
        texture = obj->groundTile;
        break;
      case TILE_BRICK:
        texture = obj->brickTile;
        break;
      }

      if (texture) {
        SDL_RenderCopy(res->renderer, texture, NULL, &tileRect);
      }
    }
    tileRect.x = 0;
  }
}

bool Initialization(struct Resources *res) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stderr, "SDL failed at initialization. Error: %s", SDL_GetError());
    return false;
  }

  res->window = SDL_CreateWindow("Super Mario Clone", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, LENGTH, HEIGHT, 0);
  if (!res->window) {
    fprintf(stderr, "Window creation failed. Error: %s", SDL_GetError());
    return false;
  }

  res->renderer = SDL_CreateRenderer(res->window, -1, 0);
  if (!res->renderer) {
    fprintf(stderr, "Renderer creation failed. Error: %s", SDL_GetError());
    return false;
  }

  int img_init = IMG_Init(IMG_INIT_PNG);
  if ((img_init & IMG_INIT_PNG) != IMG_INIT_PNG) {
    fprintf(stderr, "Image loading failed. Error: %s", IMG_GetError());
    return false;
  }

  return true;
}

bool LoadData(struct Resources *res, Objects *obj) {
  res->background = IMG_LoadTexture(res->renderer, "art/background.png");
  if (!res->background) {
    fprintf(stderr, "Background image not found. Error: %s", IMG_GetError());
    return false;
  }

  obj->groundTile = IMG_LoadTexture(res->renderer, "art/brick.png");
  if (!obj->groundTile) {
    fprintf(stderr, "Ground tile image not found. Error: %s", IMG_GetError());
    return false;
  }

  // Add more textures as needed
  obj->brickTile = IMG_LoadTexture(res->renderer, "art/brick.png");
  if (!obj->brickTile) {
    fprintf(stderr, "Brick tile image not found. Error: %s", IMG_GetError());
    return false;
  }

  return true;
}

void TerminateProgram(struct Resources *res) {
  SDL_DestroyTexture(res->background);
  SDL_DestroyRenderer(res->renderer);
  SDL_DestroyWindow(res->window);
  SDL_Quit();
}
