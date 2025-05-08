#include "list.h"
#include "render.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool running = true;

int main(void) {

  Res resources = {
      .window = NULL,
      .renderer = NULL,
  };

  struct Button TestButton = {.background = NULL, .position = NULL};

  InitProgram(&resources);
  List *list = (List *)malloc(sizeof(List));
  InitList(list);

  // main state loop
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case (SDL_QUIT):
        Deallocate(list);
        CleanupProgram(&resources);
        break;
      default:
        break;
      }
    }
    SDL_SetRenderDrawColor(resources.renderer, 255, 0, 0, 0);
    SDL_RenderPresent(resources.renderer);
    SDL_RenderClear(resources.renderer);

    SDL_Delay(REFRESHRATE);
  }
  PrintSimpleList(list);
  Deallocate(list);
  CleanupProgram(&resources);
  return 0;
}
