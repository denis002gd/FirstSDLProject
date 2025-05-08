#include "render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

int InitProgram(Res *resources) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stderr, "Error at initialization, Error: %s\n", SDL_GetError());
    return 1;
  }
  resources->window =
      SDL_CreateWindow("Visualizator SDA", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  if (!resources->window) {
    fprintf(stderr, "Error on window initialization, Error: %s\n",
            SDL_GetError());
    return 1;
  }
  resources->renderer = SDL_CreateRenderer(resources->window, -1, 0);
  if (!resources->renderer) {
    fprintf(stderr, "Error on renderer initialization, Error: %s\n",
            SDL_GetError());
    return 1;
  }
  // return 0 on success
  return 0;
}
// im lost haha
int InitButton(Res *resources, struct Button *button, int width, int height) {
  button->background =
      SDL_CreateTexture(resources->renderer, SDL_RENDERER_ACCELERATED,
                        SDL_TEXTUREACCESS_STATIC, width, height);
  if (!button->background) {
    fprintf(stderr, "Failed at creating button, Error: %s\n", SDL_GetError());
    return 1;
  }
  button->position = {200, 200, 200, 200};
  // return 0 on success
  return 0;
}

void DrawButton(Res *resources, struct Button *button) {
  SDL_RenderCopy(resources->renderer, button->background, 0, button->position);
}
void CleanupProgram(Res *resources) {
  SDL_DestroyRenderer(resources->renderer);
  SDL_DestroyWindow(resources->window);
  SDL_Quit();
}
