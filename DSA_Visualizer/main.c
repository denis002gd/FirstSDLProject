#include "list.h"
#include "render.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool running = true;
//"/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
int main(void) {

  Res resources = {
      .window = NULL,
      .renderer = NULL,
      .bgColor = NULL,
  };

  InitProgram(&resources);
  // List *list = (List *)malloc(sizeof(List));
  // InitList(list);
  int numOfButtons = 3;
  struct Button Buttons[numOfButtons];
  char font[52] = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";

  struct Button TestButton = {.background = NULL,
                              .isCLicked = 0,
                              .position = {10, 5, 200, 150},
                              .textColor = {0, 0, 0, 255}};
  struct Button TestButton2 = {.background = NULL,
                               .isCLicked = 0,
                               .position = {220, 5, 200, 150},
                               .textColor = {50, 50, 50, 255}};
  struct Button TestButton3 = {.background = NULL,
                               .isCLicked = 0,
                               .position = {430, 5, 200, 150},
                               .textColor = {100, 100, 100, 255}};
  InitButton(&resources, &TestButton, "textures/buttonReady.png",
             "textures/button.png");
  InitButtonText(&resources, &TestButton, "Button 1", 24, font);
  InitButton(&resources, &TestButton2, "textures/buttonReady.png",
             "textures/button.png");

  InitButtonText(&resources, &TestButton2, "Button 2", 24, font);
  InitButton(&resources, &TestButton3, "textures/buttonReady.png",
             "textures/button.png");

  InitButtonText(&resources, &TestButton3, "Button 3", 24, font);

  Buttons[0] = TestButton;
  Buttons[1] = TestButton2;
  Buttons[2] = TestButton3;

  int mouseX, mouseY;
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
        if (IsInsideBox(Buttons[0].position.x, Buttons[0].position.y,
                        Buttons[0].position.w, Buttons[0].position.h, mouseX,
                        mouseY)) {
          printf("First button was clicked!\n");
          Buttons[0].isCLicked = 1;

        } else {
          Buttons[0].isCLicked = 0;
        }
        if (IsInsideBox(Buttons[1].position.x, Buttons[1].position.y,
                        Buttons[1].position.w, Buttons[1].position.h, mouseX,
                        mouseY)) {
          printf("Second button was clicked!\n");
          Buttons[1].isCLicked = 1;

        } else {
          Buttons[1].isCLicked = 0;
        }
        if (IsInsideBox(Buttons[2].position.x, Buttons[2].position.y,
                        Buttons[2].position.w, Buttons[2].position.h, mouseX,
                        mouseY)) {
          printf("Third button was clicked!\n");
          Buttons[2].isCLicked = 1;

        } else {
          Buttons[2].isCLicked = 0;
        }
        break;
      default:
        break;
      }
    }
    SDL_SetRenderDrawColor(resources.renderer, 253, 240, 213, 0);

    for (int i = 0; i < numOfButtons; i++) {
      DrawButton(&resources, &Buttons[i]);
    }
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
