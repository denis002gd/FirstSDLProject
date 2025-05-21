#include "list.h"
#include "render.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool running = true;

int main(void) {

  Res resources = {
      .window = NULL,
      .renderer = NULL,
      .bgColor = NULL,
  };

  InitProgram(&resources);
  // List *list = (List *)malloc(sizeof(List));
  // InitList(list);
  struct Button TestButton = {.background = NULL,
                              .position = {10, 5, 200, 150}};
  struct Button TestButton2 = {.background = NULL,
                               .position = {220, 5, 200, 150}};

  InitButton(&resources, &TestButton, "textures/buttonReady.png",
             "textures/button.png");
  InitButton(&resources, &TestButton2, "textures/buttonReady.png",
             "textures/button.png");

  int mouseX, mouseY;
  int onText;
  // main state loop
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      SDL_GetMouseState(&mouseX, &mouseY);
      switch (event.type) {
      case (SDL_QUIT):
        running = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (IsInsideBox(TestButton.position.x, TestButton.position.y,
                        TestButton.position.w, TestButton.position.h, mouseX,
                        mouseY)) {
          printf("First button was clicked!\n");
          onText = true;

        } else {
          onText = false;
        }
        if (IsInsideBox(TestButton2.position.x, TestButton2.position.y,
                        TestButton2.position.w, TestButton2.position.h, mouseX,
                        mouseY)) {
          printf("Second button was 4 clicked\n");
        }
        break;
      default:
        break;
      }
    }
    SDL_SetRenderDrawColor(resources.renderer, 253, 240, 213, 0);
    if (!onText) {
      SDL_RenderCopy(resources.renderer, TestButton.background, 0,
                     &TestButton.position);
    } else {
      SDL_RenderCopy(resources.renderer, TestButton.selectedBG, 0,
                     &TestButton.position);
    }
    SDL_RenderCopy(resources.renderer, TestButton2.background, 0,
                   &TestButton2.position);
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
