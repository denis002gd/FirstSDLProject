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

  InitProgram(&resources);
  // List *list = (List *)malloc(sizeof(List));
  // InitList(list);
  struct Button TestButton = {.background = NULL,
                              .position = {200, 100, 500, 500}};
  InitButton(&resources, &TestButton, "textures/buttonReady.png");

  // main state loop
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case (SDL_QUIT):
        //      Deallocate(list);
        CleanupProgram(&resources);
        break;
      default:
        break;
      }
    }
    SDL_SetRenderDrawColor(resources.renderer, 0, 0, 0, 0);
    SDL_RenderCopy(resources.renderer, TestButton.background, 0,
                   &TestButton.position);
    SDL_RenderPresent(resources.renderer);
    SDL_RenderClear(resources.renderer);

    SDL_Delay(REFRESHRATE);
  }
  // PrintSimpleList(list);

  // Deallocate(list);
  CleanUpButton(&TestButton);
  CleanupProgram(&resources);
  return 0;
}
