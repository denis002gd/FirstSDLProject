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

void InitializeMainButtons(Res *resources, struct Button *buttons, int count);
void InitializePanels(Res *resources, struct Panel ***panels,
                      struct Button panelButtons[][5], int numOfButtons);
void HandleMouseInput(Res *resources, SDL_Event *event, struct Button *buttons,
                      struct Panel **panels, struct Button panelButtons[][5],
                      struct InputField *inputField, int *activePanel);
void HandleKeyboardInput(SDL_Event *event, struct InputField *inputField,
                         char *inputBuffer, int *inputPos);
void RenderScene(Res *resources, struct Button *buttons, struct Panel **panels,
                 struct Button panelButtons[][5], struct InputField *inputField,
                 Node_v *nodeVis, int activePanel);
void CleanupAll(struct Button *buttons, struct Panel **panels,
                struct Button panelButtons[][5], Node_v *nodeVis,
                Res *resources);

int main(void) {
  Res resources = {0};
  InitProgram(&resources);

  const int numOfButtons = 3;
  struct Button mainButtons[numOfButtons];
  struct Panel **panels;
  struct Button panelButtons[numOfButtons][5];
  struct InputField inputField = {0};
  Node_v nodeVis = {0};

  InitializeMainButtons(&resources, mainButtons, numOfButtons);
  InitializePanels(&resources, &panels, panelButtons, numOfButtons);

  InitInputField(&resources, &inputField, (SDL_Rect){700, 850, 50, 100},
                 FONT_PATH, "Type here", 50);

  InitNode(&nodeVis, &resources, "node 1", (SDL_Rect){500, 500, 150, 90}, 1);

  int activePanel = -1;
  int inputPos = 0;
  char inputBuffer[6] = {0};

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        HandleMouseInput(&resources, &event, mainButtons, panels, panelButtons,
                         &inputField, &activePanel);
        break;
      case SDL_KEYDOWN:
        HandleKeyboardInput(&event, &inputField, inputBuffer, &inputPos);
        break;
      }
    }

    RenderScene(&resources, mainButtons, panels, panelButtons, &inputField,
                &nodeVis, activePanel);
    SDL_Delay(REFRESHRATE);
  }

  CleanupAll(mainButtons, panels, panelButtons, &nodeVis, &resources);
  return 0;
}

// initialization functions
void InitializeMainButtons(Res *resources, struct Button *buttons, int count) {
  const char *buttonTexts[] = {"Linked List", "Stack", "Queue"};
  const int fontSizes[] = {22, 30, 29};

  for (int i = 0; i < count; i++) {
    buttons[i] = (struct Button){
        .background = NULL,
        .isCLicked = 0,
        .isActive = 1,
        .position = {10 + i * (BUTTON_SPACING + BUTTON_WIDTH), 5, BUTTON_WIDTH,
                     BUTTON_HEIGHT},
        .textColor = {0, 0, 0, 255},
    };
    InitButton(resources, &buttons[i], "textures/buttonReady.png",
               "textures/button.png");
    InitButtonText(resources, &buttons[i], buttonTexts[i], fontSizes[i],
                   FONT_PATH);
  }
}

void InitializePanels(Res *resources, struct Panel ***panels,
                      struct Button panelButtons[][5], int numOfButtons) {
  const int panelButtonsCount[] = {5, 4, 3};
  const char *panelTexts[][5] = {
      {"Create Node", "Insert Node", "Traverse List", "Sort List",
       "Delete Node"},
      {"Push Element", "Pop Element", "Peek", "Clear Stack"},
      {"Enqueue Item", "Dequeue Item", "Clear Queue"}};
  const int panelFontSizes[] = {20, 20, 20, 20, 20};

  *panels = malloc(numOfButtons * sizeof(struct Panel *));

  for (int i = 0; i < numOfButtons; i++) {
    (*panels)[i] = malloc(sizeof(struct Panel));
    (*panels)[i]->buttonsCount = panelButtonsCount[i];
    (*panels)[i]->isActive = 0;
    (*panels)[i]->panelIndex = i;
    InitPanel(resources, (*panels)[i]);

    for (int j = 0; j < panelButtonsCount[i]; j++) {
      panelButtons[i][j] = (struct Button){
          .background = NULL,
          .isCLicked = 0,
          .isActive = 1,
          .position = {50, HEIGHT - j * (25 + BUTTON_HEIGHT) - 140,
                       BUTTON_WIDTH, BUTTON_HEIGHT},
          .textColor = {0, 0, 0, 255},
      };
      InitButton(resources, &panelButtons[i][j], "textures/buttonReady.png",
                 "textures/button.png");
      InitButtonText(resources, &panelButtons[i][j], panelTexts[i][j],
                     panelFontSizes[j], FONT_PATH);
    }
  }
}

void HandleMouseInput(Res *resources, SDL_Event *event, struct Button *buttons,
                      struct Panel **panels, struct Button panelButtons[][5],
                      struct InputField *inputField, int *activePanel) {
  static const int numOfButtons = 3;
  static const int panelButtonsCount[] = {5, 4, 3};
  static const char *buttonTexts[] = {"Linked List", "Stack", "Queue"};
  static const char *panelTexts[][5] = {
      {"Create Node", "Insert Node", "Traverse List", "Sort List",
       "Delete Node"},
      {"Push Element", "Pop Element", "Peek", "Clear Stack"},
      {"Enqueue Item", "Dequeue Item", "Clear Queue"}};

  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);

  if (event->type == SDL_MOUSEBUTTONDOWN) {
    // Handle input field click
    if (IsInsideBox(inputField->bgPos.x, inputField->bgPos.y,
                    inputField->bgPos.w, inputField->bgPos.h, mouseX, mouseY)) {
      inputField->active = true;
    } else {
      inputField->active = false;
    }

    // Handle main button clicks
    for (int i = 0; i < numOfButtons; i++) {
      if (buttons[i].isActive &&
          IsInsideBox(buttons[i].position.x, buttons[i].position.y,
                      buttons[i].position.w, buttons[i].position.h, mouseX,
                      mouseY)) {
        printf("%s button was clicked!\n", buttonTexts[i]);
        PlayAudio(resources, 1);
        // Deactivate all panels
        for (int k = 0; k < numOfButtons; k++) {
          panels[k]->isActive = 0;
          buttons[k].isCLicked = 0;
        }

        // Activate clicked panel
        buttons[i].isCLicked = 1;
        panels[i]->isActive = 1;
        *activePanel = i;
        break;
      }
    }

    // Handle panel button clicks
    if (*activePanel >= 0 && panels[*activePanel]->isActive) {
      for (int j = 0; j < panelButtonsCount[*activePanel]; j++) {
        if (IsInsideBox(panelButtons[*activePanel][j].position.x,
                        panelButtons[*activePanel][j].position.y,
                        panelButtons[*activePanel][j].position.w,
                        panelButtons[*activePanel][j].position.h, mouseX,
                        mouseY)) {
          printf("Panel button '%s' was clicked!\n",
                 panelTexts[*activePanel][j]);
          panelButtons[*activePanel][j].isCLicked = 1;
        } else {
          panelButtons[*activePanel][j].isCLicked = 0;
        }
      }
    }
  } else if (event->type == SDL_MOUSEBUTTONUP) {
    // Reset all panel button states
    for (int i = 0; i < numOfButtons; i++) {
      for (int j = 0; j < panelButtonsCount[i]; j++) {
        panelButtons[i][j].isCLicked = 0;
      }
    }
  }
}

void HandleKeyboardInput(SDL_Event *event, struct InputField *inputField,
                         char *inputBuffer, int *inputPos) {
  if (!inputField->active)
    return;

  SDL_Keycode key = event->key.keysym.sym;

  if (key >= SDLK_0 && key <= SDLK_9 && *inputPos < 5) {
    inputBuffer[(*inputPos)++] = (char)key;
    inputBuffer[*inputPos] = '\0';
    printf("Input: %s\n", inputBuffer);
  } else if (key == SDLK_BACKSPACE && *inputPos > 0) {
    inputBuffer[--(*inputPos)] = '\0';
    printf("Input: %s\n", inputBuffer);
  } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
    // int val = atoi(inputBuffer);
    inputField->active = false;
    // Handle the entered value here
  }

  inputField->input = inputBuffer;
}

void RenderScene(Res *resources, struct Button *buttons, struct Panel **panels,
                 struct Button panelButtons[][5], struct InputField *inputField,
                 Node_v *nodeVis, int activePanel) {
  static const int numOfButtons = 3;
  static const int panelButtonsCount[] = {5, 4, 3};

  SDL_SetRenderDrawColor(resources->renderer, 253, 240, 213, 0);
  SDL_RenderClear(resources->renderer);

  UpdateList(resources, nodeVis);

  // Draw main buttons
  for (int i = 0; i < numOfButtons; i++) {
    if (buttons[i].isActive) {
      DrawButton(resources, &buttons[i]);
    }
  }

  // Draw active panel and its buttons
  if (activePanel >= 0 && panels[activePanel]->isActive) {
    RenderPanel(resources, panels[activePanel]);
    for (int j = 0; j < panelButtonsCount[activePanel]; j++) {
      DrawButton(resources, &panelButtons[activePanel][j]);
      panelButtons[activePanel][j].isActive = 0;
    }
  }

  UpdateInput(resources, inputField, inputField->input);
  SDL_RenderPresent(resources->renderer);
}

void CleanupAll(struct Button *buttons, struct Panel **panels,
                struct Button panelButtons[][5], Node_v *nodeVis,
                Res *resources) {
  static const int numOfButtons = 3;
  static const int panelButtonsCount[] = {5, 4, 3};

  for (int i = 0; i < numOfButtons; i++) {
    CleanUpButton(&buttons[i]);

    for (int j = 0; j < panelButtonsCount[i]; j++) {
      CleanUpButton(&panelButtons[i][j]);
    }

    if (panels[i]) {
      CleanupPanel(panels[i]);
      panels[i] = NULL;
    }
  }

  FreeNodesInfo(nodeVis);

  free(panels);

  CleanupProgram(resources);
}
