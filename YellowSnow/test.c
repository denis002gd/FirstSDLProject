#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define HEIGHT 1080
#define LENGTH 1920
#define TITLE "VIZUALIZARE SORTARE"
#define VSYNC 16

struct Game {
  SDL_Window *window;
  SDL_Renderer *renderer;
};
bool Initialize(struct Game *game) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stdout, "SDL a esuat la initializare: %s\n", SDL_GetError());
    return true;
  }
  game->window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, LENGTH, HEIGHT, 0);
  if (!game->window) {
    fprintf(stdout, "SDL a esuat la crearea ferestrei: %s\n", SDL_GetError());
    return true;
  }
  game->renderer =
      SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
  if (!game->renderer) {
    fprintf(stdout, "SDL a esuat la crearea renderului: %s\n", SDL_GetError());
    return true;
  }
  return false;
}
void DrawLine(struct Game *game, int x1, int y1, int x2, int y2) {
  if (x1 > 0 && x1 < LENGTH && y1 > 0 && y1 < HEIGHT) {
    if (x2 > 0 && x2 < LENGTH && y2 > 0 && y2 < HEIGHT) {
      SDL_RenderDrawLine(game->renderer, x1, y1, x2, y2);
    }
  }
}
void QuitApp(struct Game *game) {
  SDL_DestroyRenderer(game->renderer);
  SDL_DestroyWindow(game->window);
  SDL_Quit();
}

int main(void) {

  struct Game game = {
      .window = NULL,
      .renderer = NULL,
  };
  int mouseDrag[5];
  memset(mouseDrag, 0, 5);

  if (Initialize(&game)) {
    QuitApp(&game);
  }

  while (true) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case:

      case SDL_MOUSEBUTTONDOWN:
        mouseDrag[0] = event.button.x;
        mouseDrag[1] = event.button.y;
      case SDL_MOUSEBUTTONUP:
        mouseDrag[2] = event.button.x;
        mouseDrag[3] = event.button.y;
        DrawLine(&game, mouseDrag[0], mouseDrag[1], mouseDrag[2], mouseDrag[3]);
      default:
        break;
      }
    }
    SDL_SetRenderDrawColor(game.renderer, 10, 0, 0, 255);
    SDL_RenderClear(game.renderer);
    SDL_RenderPresent(game.renderer);

    SDL_Delay(VSYNC);
  }

  return 0;
}
