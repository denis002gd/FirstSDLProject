#include "buttons.h"
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
#include <time.h>

bool running = true;
bool startLLtraversal = false;
static char popupBuffer[5] = {0};
static int popupPos = 0;
static int isTraversing = 0;
static int traverseIndex = 0;
static double traverseTimer = 0.0;
double deltaTime = 0;
Node_v *listHead = NULL;
Vector2 middle = {500, 250};
struct PopupPanel valueInputPanel;

void InitializeMainButtons(Res *resources, struct Button *buttons, int count);
void InitializePanels(Res *resources, struct Panel ***panels,
                      struct Button panelButtons[][5], int numOfButtons);
void HandleMouseInput(Res *resources, SDL_Event *event, struct Button *buttons,
                      struct Panel **panels, struct Button panelButtons[][5],
                      struct InputField *inputField, int *activePanel,
                      Node_v *list);
void HandleKeyboardInput(Res *resources, SDL_Event *event,
                         struct InputField *inputField, char *inputBuffer,
                         int *inputPos);
void RenderScene(Res *resources, struct Button *buttons, struct Panel **panels,
                 struct Button panelButtons[][5], struct InputField *inputField,
                 Node_v *nodeVis, int activePanel);
void CleanupAll(struct Button *buttons, struct Panel **panels,
                struct Button panelButtons[][5], Node_v **listHead,
                Res *resources);

int main(void) {
  srand(time(NULL));
  Res resources = {0};
  InitProgram(&resources);

  const int numOfButtons = 3;
  struct Button mainButtons[numOfButtons];
  struct Panel **panels;
  struct Button panelButtons[numOfButtons][5];
  struct InputField inputField = {0};
  int activePanel = -1;
  int inputPos = 0;
  char inputBuffer[6] = {0};

  InitializeMainButtons(&resources, mainButtons, numOfButtons);
  InitializePanels(&resources, &panels, panelButtons, numOfButtons);
  InitPopPanel(&resources, &valueInputPanel, 350, 780, 250, 190);

  Uint64 last = 0;
  Uint64 now = SDL_GetPerformanceCounter();

  while (running) {
    last = now;
    now = SDL_GetPerformanceCounter();
    deltaTime = (double)((now - last) * 1000) / SDL_GetPerformanceFrequency();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        HandleMouseInput(&resources, &event, mainButtons, panels, panelButtons,
                         &inputField, &activePanel, listHead);
        break;
      case SDL_KEYDOWN:
        HandleKeyboardInput(&resources, &event, &inputField, inputBuffer,
                            &inputPos);
        break;
      }
    }

    if (isTraversing && listHead != NULL) {
      traverseTimer += deltaTime;
      if (traverseTimer >= 1000.0) {
        traverseTimer = 0.0;

        Node_v *temp = listHead;
        while (temp != NULL) {
          temp->isSelected = false;
          temp = temp->next;
        }

        if (traverseIndex < resources.nodesNumber) {
          int targetIndex = resources.nodesNumber - 1 - traverseIndex;
          temp = listHead;
          int nodeIndex = 0;
          while (temp != NULL && nodeIndex < targetIndex) {
            temp = temp->next;
            nodeIndex++;
          }
          if (temp != NULL) {
            temp->isSelected = true;
          }
          traverseIndex++;
        } else {
          isTraversing = 0;
          traverseIndex = 0;
        }
      }
    }

    RenderScene(&resources, mainButtons, panels, panelButtons, &inputField,
                listHead, activePanel);
    SDL_Delay(REFRESHRATE);
  }

  CleanupPopPanel(&valueInputPanel);
  CleanupAll(mainButtons, panels, panelButtons, &listHead, &resources);
  return 0;
}

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
                      struct InputField *inputField, int *activePanel,
                      Node_v *list) {
  const int numOfButtons = 3;
  const int panelButtonsCount[] = {5, 4, 3};
  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);

  CheckList(listHead, mouseX, mouseY, (event->type == SDL_MOUSEBUTTONDOWN));

  if (event->type == SDL_MOUSEBUTTONDOWN) {
    if (valueInputPanel.isActive) {
      if (IsAddButtonClicked(&valueInputPanel, mouseX, mouseY)) {
        if (valueInputPanel.input && strlen(valueInputPanel.input) > 0) {
          int value = atoi(valueInputPanel.input);
          Node_v *newNode = AddNodeToList(resources, &listHead, value);
          if (newNode) {
            PlayAudio(resources, 3);
          } else {
            PlayAudio(resources, 2);
          }
        }
        popupPos = 0;
        popupBuffer[0] = '\0';
        HandlePopPanelInput(&valueInputPanel, "Type Here");
        HidePopPanel(&valueInputPanel);
        return;
      }
      if (IsInputFieldClicked(&valueInputPanel, mouseX, mouseY)) {
        valueInputPanel.inputField->active = true;
        return;
      }
      if (!IsInsideBox(valueInputPanel.position.x, valueInputPanel.position.y,
                       valueInputPanel.position.w, valueInputPanel.position.h,
                       mouseX, mouseY)) {
        popupPos = 0;
        popupBuffer[0] = '\0';
        HidePopPanel(&valueInputPanel);
        return;
      }
    }

    for (int i = 0; i < numOfButtons; i++) {
      if (buttons[i].isActive &&
          IsInsideBox(buttons[i].position.x, buttons[i].position.y,
                      buttons[i].position.w, buttons[i].position.h, mouseX,
                      mouseY)) {
        PlayAudio(resources, 1);
        for (int k = 0; k < numOfButtons; k++) {
          panels[k]->isActive = 0;
          buttons[k].isCLicked = 0;
        }
        buttons[i].isCLicked = 1;
        panels[i]->isActive = 1;
        *activePanel = i;
        break;
      }
    }

    if (*activePanel >= 0 && panels[*activePanel]->isActive) {
      for (int j = 0; j < panelButtonsCount[*activePanel]; j++) {
        if (IsInsideBox(panelButtons[*activePanel][j].position.x,
                        panelButtons[*activePanel][j].position.y,
                        panelButtons[*activePanel][j].position.w,
                        panelButtons[*activePanel][j].position.h, mouseX,
                        mouseY)) {
          panelButtons[*activePanel][j].isCLicked = 1;

          if (j == 0 && (*activePanel == 0 || *activePanel == 1)) {
            popupPos = 0;
            popupBuffer[0] = '\0';
            ShowPopPanel(&valueInputPanel);
            HandlePopPanelInput(&valueInputPanel, "Type here");
          }

          if (j == 2 && *activePanel == 0) {
            isTraversing = 1;
            traverseIndex = 0;
            traverseTimer = 0.0;
            Node_v *temp = listHead;
            while (temp != NULL) {
              temp->isSelected = false;
              temp = temp->next;
            }
          }

          PlayAudio(resources, 1);
        } else {
          panelButtons[*activePanel][j].isCLicked = 0;
        }
      }
    }
  } else if (event->type == SDL_MOUSEBUTTONUP) {
    for (int i = 0; i < numOfButtons; i++) {
      for (int j = 0; j < panelButtonsCount[i]; j++) {
        panelButtons[i][j].isCLicked = 0;
      }
    }
  }
}

void HandleKeyboardInput(Res *resources, SDL_Event *event,
                         struct InputField *inputField, char *inputBuffer,
                         int *inputPos) {
  SDL_Keycode key = event->key.keysym.sym;

  if (key == SDLK_BACKSLASH && listHead) {
    listHead->isMoving = true;
  }

  if (valueInputPanel.isActive && valueInputPanel.inputField &&
      valueInputPanel.inputField->active) {
    if (key >= SDLK_0 && key <= SDLK_9 && popupPos < 31) {
      popupBuffer[popupPos++] = (char)key;
      popupBuffer[popupPos] = '\0';
      HandlePopPanelInput(&valueInputPanel, popupBuffer);
    } else if (key == SDLK_BACKSPACE && popupPos > 0) {
      popupBuffer[--popupPos] = '\0';
      HandlePopPanelInput(&valueInputPanel, popupBuffer);
    } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
      if (strlen(popupBuffer) > 0) {
        int value = atoi(popupBuffer);
        Node_v *newNode = AddNodeToList(resources, &listHead, value);
        if (newNode) {
          PlayAudio(resources, 3);
        } else {
          PlayAudio(resources, 2);
        }
      }
      popupPos = 0;
      popupBuffer[0] = '\0';
      HandlePopPanelInput(&valueInputPanel, "Type here");
      HidePopPanel(&valueInputPanel);
    } else if (key == SDLK_ESCAPE) {
      popupPos = 0;
      popupBuffer[0] = '\0';
      HandlePopPanelInput(&valueInputPanel, "Type here");
      HidePopPanel(&valueInputPanel);
    }
  }
}

void RenderScene(Res *resources, struct Button *buttons, struct Panel **panels,
                 struct Button panelButtons[][5], struct InputField *inputField,
                 Node_v *listHead, int activePanel) {
  const int numOfButtons = 3;
  const int panelButtonsCount[] = {5, 4, 3};

  SDL_SetRenderDrawColor(resources->renderer, 253, 240, 213, 0);
  SDL_RenderClear(resources->renderer);
  ScrollBg(resources, deltaTime, 0.1f);
  UpdateList(resources, listHead);

  for (int i = 0; i < numOfButtons; i++) {
    if (buttons[i].isActive) {
      DrawButton(resources, &buttons[i]);
    }
  }

  if (activePanel >= 0 && panels[activePanel]->isActive) {
    RenderPanel(resources, panels[activePanel]);
    for (int j = 0; j < panelButtonsCount[activePanel]; j++) {
      DrawButton(resources, &panelButtons[activePanel][j]);
      panelButtons[activePanel][j].isActive = 0;
    }
  }

  RenderPopPanel(resources, &valueInputPanel);
  SDL_RenderPresent(resources->renderer);
}

void CleanupAll(struct Button *buttons, struct Panel **panels,
                struct Button panelButtons[][5], Node_v **listHead,
                Res *resources) {
  const int numOfButtons = 3;
  const int panelButtonsCount[] = {5, 4, 3};

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

  FreeNodesInfo(listHead);
  free(panels);
  CleanupProgram(resources);
}
