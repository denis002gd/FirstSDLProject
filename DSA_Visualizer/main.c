#include "list.h"
#include "render.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
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
      .clickSFX = NULL,
      .errorSFX = NULL,
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

  int maxNumPanels = 5;
  const int panelButtonsCount[] = {5, 4, 3}; // each panel has 1 button
  struct Panel **Panels = malloc(numOfButtons * sizeof(struct Panel *));

  // panel button texts for each panel
  const char *panelTexts[][5] = {
      {"Create Node", "Insert Node", "Traverse List", "Sort List",
       "Delete Node"},
      {"Push Element", "Pop Element", "Peek", "Clear Stack"},
      {"Enqueue Item", "Dequeue Item", "Clear Queue"}};
  const int panelFontSizes[] = {20, 20, 20, 20, 20};

  struct Button panelButtons[numOfButtons][maxNumPanels];

  for (int i = 0; i < numOfButtons; i++) {

    Panels[i] = malloc(sizeof(struct Panel));
    if (Panels[i] == NULL) {
      printf("Failed to allocate memory for panel %d\n", i);
      exit(1);
    }
    Panels[i]->buttonsCount = panelButtonsCount[i];
    InitPanel(&resources, Panels[i]);
    Panels[i]->isActive = 0;
    Panels[i]->panelIndex = i;

    for (int j = 0; j < panelButtonsCount[i]; j++) {
      panelButtons[i][j] = (struct Button){
          .background = NULL,
          .isCLicked = 0,
          .isActive = 1,
          .position = {50, HEIGHT - j * (25 + (BUTTON_HEIGHT)) - 140,
                       BUTTON_WIDTH, BUTTON_HEIGHT},
          .textColor = {0, 0, 0, 255},
      };
      InitButton(&resources, &panelButtons[i][j], "textures/buttonReady.png",
                 "textures/button.png");
      InitButtonText(&resources, &panelButtons[i][j], panelTexts[i][j],
                     panelFontSizes[j], FONT_PATH);
    }
  }
  //===============================================================================================

  struct InputField inputField = {
      .textTexture = NULL,
      .font = NULL,
      .input = NULL,
      .position = (SDL_Rect){300, 300, 50, 50},
      .active = 0,
      .background = NULL,
      .fontSize = 20,
  };
  InitInputField(&resources, &inputField, (SDL_Rect){700, 850, 50, 100},
                 FONT_PATH, "Type here", 50);
  int mouseX, mouseY;
  int activePanel = -1; // track if a panel is currently active -1 = none
  int inputPos = 0;
  char inputBuffer[6] = {0};
  // Main game loop
  while (running) {
    SDL_Event event;
    SDL_Keycode key;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
        // TODO: move the buttons checking in separate functions for readability
      case SDL_MOUSEBUTTONDOWN:
        SDL_GetMouseState(&mouseX, &mouseY);

        if (IsInsideBox(inputField.bgPos.x, inputField.bgPos.y,
                        inputField.bgPos.w, inputField.bgPos.h, mouseX,
                        mouseY)) {
          inputField.active = true;
        } else {
          inputField.active = false;
        }

        for (int i = 0; i < numOfButtons; i++) {
          if (Buttons[i].isActive == 1) {
            if (IsInsideBox(Buttons[i].position.x, Buttons[i].position.y,
                            Buttons[i].position.w, Buttons[i].position.h,
                            mouseX, mouseY)) {
              printf("%s button was clicked!\n", buttonTexts[i]);
              PlayAudio(&resources, 1);

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

              PlayAudio(&resources, 2);
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
        break;
      case SDL_KEYDOWN:
        key = event.key.keysym.sym;
        if (inputField.active) {
          if (key >= SDLK_0 && key <= SDLK_9 &&
              inputPos < sizeof(inputBuffer) - 1) {
            inputBuffer[inputPos++] = (char)(key);
            inputBuffer[inputPos] = '\0';
            printf("Input: %s\n", inputBuffer);
          }
          if (key == SDLK_BACKSPACE && inputPos > 0) {
            inputBuffer[--inputPos] = '\0';
            printf("Input: %s\n", inputBuffer);
          }
          inputField.input = inputBuffer;
          if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {

            int val = atoi(inputBuffer);
            inputField.active = false;
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

    UpdateInput(&resources, &inputField, inputField.input);
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
