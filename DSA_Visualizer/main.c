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

int main(void) {
  Res resources = {
      .window = NULL,
      .renderer = NULL,
      .bgColor = NULL,
  };
  InitProgram(&resources);

  // main navigation buttons
  int numOfButtons = 3;
  struct Button Buttons[numOfButtons];
  const char *buttonTexts[] = {"Linked List", "Stack", "Queue"};
  const int fontSizes[] = {22, 30, 29};

  // initialize main buttons
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

  int maxNumPanels = 3;
  const int panelButtonsCount[] = {3, 1, 2}; // each panel has 1 button
  struct Panel **Panels = malloc(numOfButtons * sizeof(struct Panel *));

  // panel button texts for each panel
  const char *panelTexts[][3] = {{"Create Node", "Insert Node", "Delete Node"},
                                 {"Push Element"},
                                 {"Enqueue Item", "Dequeue Item"}};
  const int panelFontSizes[] = {20, 20, 20};

  struct Button panelButtons[numOfButtons][maxNumPanels];

  for (int i = 0; i < numOfButtons; i++) {

    Panels[i] = malloc(sizeof(struct Panel));
    if (Panels[i] == NULL) {
      printf("Failed to allocate memory for panel %d\n", i);
      exit(1);
    }

    InitPanel(&resources, Panels[i]);
    Panels[i]->isActive = 0;
    Panels[i]->panelIndex = i;

    for (int j = 0; j < panelButtonsCount[i]; j++) {
      panelButtons[i][j] = (struct Button){
          .background = NULL,
          .isCLicked = 0,
          .isActive = 1,
          .position = {50, 530 + j * ((BUTTON_SPACING / 2) + BUTTON_HEIGHT),
                       BUTTON_WIDTH, BUTTON_HEIGHT},
          .textColor = {0, 0, 0, 255},
      };

      InitButton(&resources, &panelButtons[i][j], "textures/buttonReady.png",
                 "textures/button.png");
      InitButtonText(&resources, &panelButtons[i][j], panelTexts[i][j],
                     panelFontSizes[j], FONT_PATH);
    }
  }

  int mouseX, mouseY;
  int activePanel = -1; // track if a panel is currently active -1 = none

  // Main game loop
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;

      case SDL_MOUSEBUTTONDOWN:
        SDL_GetMouseState(&mouseX, &mouseY);

        for (int i = 0; i < numOfButtons; i++) {
          if (Buttons[i].isActive == 1) {
            if (IsInsideBox(Buttons[i].position.x, Buttons[i].position.y,
                            Buttons[i].position.w, Buttons[i].position.h,
                            mouseX, mouseY)) {
              printf("%s button was clicked!\n", buttonTexts[i]);

              // deactivate all panels first
              for (int k = 0; k < numOfButtons; k++) {
                Panels[k]->isActive = 0;
                Buttons[k].isCLicked = 0;
              }

              // activate the clicked panel
              Buttons[i].isCLicked = 1;
              Panels[i]->isActive = 1;
              activePanel = i;
            }
          }
        }

        if (activePanel >= 0 && Panels[activePanel]->isActive) {
          for (int j = 0; j < panelButtonsCount[activePanel]; j++) {
            if (IsInsideBox(panelButtons[activePanel][j].position.x,
                            panelButtons[activePanel][j].position.y,
                            panelButtons[activePanel][j].position.w,
                            panelButtons[activePanel][j].position.h, mouseX,
                            mouseY)) {
              printf("Panel button '%s' was clicked!\n",
                     panelTexts[activePanel][j]);
              panelButtons[activePanel][j].isCLicked = 1;

            } else {
              panelButtons[activePanel][j].isCLicked = 0;
            }
          }
        }
        break;
      case SDL_MOUSEBUTTONUP:
        for (int i = 0; i < numOfButtons; i++) {
          for (int j = 0; j < panelButtonsCount[i]; j++) {
            panelButtons[i][j].isCLicked = 0;
          }
        }
      default:
        break;
      }
    }

    SDL_SetRenderDrawColor(resources.renderer, 253, 240, 213, 0);
    SDL_RenderClear(resources.renderer);

    for (int i = 0; i < numOfButtons; i++) {
      if (Buttons[i].isActive == 1) {
        DrawButton(&resources, &Buttons[i]);
      }
    }

    if (activePanel >= 0 && Panels[activePanel]->isActive) {
      RenderPanel(&resources, Panels[activePanel]);

      for (int j = 0; j < panelButtonsCount[activePanel]; j++) {
        DrawButton(&resources, &panelButtons[activePanel][j]);
        panelButtons[activePanel][j].isActive = 0;
      }
    }

    SDL_RenderPresent(resources.renderer);
    SDL_Delay(REFRESHRATE);
  }
  // cleanup
  for (int i = 0; i < numOfButtons; i++) {
    CleanUpButton(&Buttons[i]);

    for (int j = 0; j < panelButtonsCount[i]; j++) {
      CleanUpButton(&panelButtons[i][j]);
    }

    if (Panels[i]) {
      CleanupPanel(Panels[i]);
    }
  }

  free(Panels);
  CleanupProgram(&resources);
  return 0;
}
