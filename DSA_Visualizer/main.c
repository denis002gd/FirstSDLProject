#include "list.h"
#include "render.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
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
  const char *buttonTexts[] = {"Linked List", "Stack", "Queue"};
  const int fontSizes[] = {22, 30, 29};

  struct Panel *listPanel = malloc(sizeof(struct Panel));
  InitPanel(&resources, listPanel);
  listPanel->isActive = 1;
  for (int i = 0; i < numOfButtons; i++) {
    Buttons[i] = (struct Button){
        .background = NULL,
        .isCLicked = 0,
        .isActive = 1,
        .position = {10 + i * (BUTTON_SPACING + BUTTON_WIDTH), 5, BUTTON_WIDTH,
                     BUTTON_HEIGHT},
        .textColor = {0, 0, 0, 255},
    };
    InitButton(&resources, &Buttons[i], "textures/buttonReady.png",
               "textures/button.png");
    InitButtonText(&resources, &Buttons[i], buttonTexts[i], fontSizes[i],
                   FONT_PATH);
  }

  int mouseX, mouseY;
  // main state loop
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case (SDL_QUIT):
        running = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        SDL_GetMouseState(&mouseX, &mouseY);
        for (int i = 0; i < numOfButtons; i++) {
          if (Buttons[i].isActive == 1) {
            if (IsInsideBox(Buttons[i].position.x, Buttons[i].position.y,
                            Buttons[i].position.w, Buttons[i].position.h,
                            mouseX, mouseY)) {
              printf("%s button was clicked!\n", Buttons[i].text);
              Buttons[i].isCLicked = 1;

            } else {
              Buttons[i].isCLicked = 0;
            }
          }
        }
        break;
      default:
        break;
      }
    }
    SDL_SetRenderDrawColor(resources.renderer, 253, 240, 213, 0);

    SDL_RenderClear(resources.renderer);
    for (int i = 0; i < numOfButtons; i++) {
      if (Buttons[i].isActive == 1)
        DrawButton(&resources, &Buttons[i]);
    }
    if (Buttons[0].isCLicked == 1) {
      listPanel->isActive = 1;
      RenderPanel(&resources, listPanel);
    } else {
      listPanel->isActive = 0;
    }
    SDL_RenderPresent(resources.renderer);

    SDL_Delay(REFRESHRATE);
  }
  // PrintSimpleList(list);

  // Deallocate(list);
  for (int i = 0; i < numOfButtons; i++) {
    CleanUpButton(&Buttons[i]);
  }
  CleanupPanel(listPanel);
  CleanupProgram(&resources);
  return 0;
}
