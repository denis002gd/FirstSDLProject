#include "render.h"
#include "list.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// ============================ProgramEssentials=============================
int InitProgram(Res *resources) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stderr, "Error at initialization, Error: %s\n", SDL_GetError());
    return 1;
  }
  TTF_Init();
  if (IMG_Init(IMG_INIT_PNG) == 0) {
    fprintf(stderr, "Error on image initialization, Error: %s", IMG_GetError());
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
  if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) < 0) {
    fprintf(stdout, "Failed audio initialization, Error: %s\n", Mix_GetError());
    return 1;
  }
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    fprintf(stdout, "SDL_mixer could not open audio! Error: %s\n",
            Mix_GetError());
    return 1;
  }
  resources->clickSFX = Mix_LoadMUS("audio/click.mp3");
  resources->errorSFX = Mix_LoadMUS("audio/error.mp3");
  resources->spawnSFX = Mix_LoadMUS("audio/spawn.mp3");
  if (resources->clickSFX == NULL || resources->errorSFX == NULL ||
      resources->spawnSFX == NULL) {
    fprintf(stdout, "SDL_mixer did not find audio file! Error: %s\n",
            Mix_GetError());
  }
  resources->renderer = SDL_CreateRenderer(resources->window, -1, 0);
  if (!resources->renderer) {
    fprintf(stderr, "Error on renderer initialization, Error: %s\n",
            SDL_GetError());
    return 1;
  }
  resources->bgTexture =
      IMG_LoadTexture(resources->renderer, "textures/background.png");
  if (!resources->bgTexture) {
    fprintf(stdout, "Failed to loadBackground, Error: \n", IMG_GetError());
    return 1;
  }
  SDL_QueryTexture(resources->bgTexture, NULL, NULL, &resources->bgW,
                   &resources->bgH);
  // return 0 on success
  return 0;
}

void CleanupProgram(Res *resources) {
  SDL_DestroyRenderer(resources->renderer);
  SDL_DestroyWindow(resources->window);
  Mix_FreeMusic(resources->clickSFX);
  Mix_FreeMusic(resources->errorSFX);
  SDL_DestroyTexture(resources->bgTexture);
  IMG_Quit();
  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
}
//===========================ButtonFunctions=================================
int InitButton(Res *resources, struct Button *button, char *path,
               char *secondPath) {

  button->background = IMG_LoadTexture(resources->renderer, path);

  if (!button->background) {
    fprintf(stderr, "Failed at creating button, Error: %s\n", SDL_GetError());
    return 1;
  }
  button->selectedBG = IMG_LoadTexture(resources->renderer, secondPath);
  if (!button->selectedBG) {
    printf("Second path not assigned!\n");
  }
  // return 0 on success
  return 0;
}

void InitButtonText(Res *res, struct Button *button, const char *text,
                    int textSize, const char *font) {
  button->font = TTF_OpenFont(font, textSize);
  if (!button->font) {
    fprintf(stderr, "Failed to open font. Error Log: %s\n", TTF_GetError());
    return;
  }
  button->text = strdup(text);
  SDL_Surface *surface =
      TTF_RenderText_Solid(button->font, text, button->textColor);
  if (!surface) {
    fprintf(stderr, "Failed to create text surface: %s\n", TTF_GetError());
    return;
  }

  button->textTexture = SDL_CreateTextureFromSurface(res->renderer, surface);
  if (!button->textTexture) {
    fprintf(stderr, "Failed to create text texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  button->textRect.w = surface->w;
  button->textRect.h = surface->h;
  button->textRect.x =
      button->position.x + (button->position.w - surface->w) / 2;
  button->textRect.y =
      button->position.y + ((button->position.h - surface->h) / 2) - 10;

  SDL_FreeSurface(surface);
}

void DrawButton(Res *resources, struct Button *button) {
  SDL_Rect drawPos = button->position;
  SDL_Rect textPos = button->textRect;

  if (!button->isCLicked) {
    SDL_RenderCopy(resources->renderer, button->background, 0, &drawPos);
  } else {
    int shrink = 3;
    drawPos.x += shrink;
    drawPos.y += shrink;
    drawPos.w -= shrink * 2;
    drawPos.h -= shrink * 2;

    textPos.x += shrink;
    textPos.y += shrink;

    SDL_RenderCopy(resources->renderer, button->selectedBG, 0, &drawPos);
  }

  SDL_RenderCopy(resources->renderer, button->textTexture, 0, &textPos);
}
void CleanUpButton(struct Button *button) {
  if (button->background)
    SDL_DestroyTexture(button->background);
  if (button->selectedBG)
    SDL_DestroyTexture(button->selectedBG);
  if (button->font) {
    TTF_CloseFont(button->font);
  }
  if (button->textTexture) {
    SDL_DestroyTexture(button->textTexture);
  }
  if (button->text) {
    free(button->text);
  }
}

//==================================PanelFunctions=============================
int InitPanel(Res *res, struct Panel *panel) {
  panel->background = IMG_LoadTexture(res->renderer, "textures/filePanel.png");
  if (!panel->background) {
    fprintf(stdout, "Texture loading error, Error log: %s\n", IMG_GetError());
    return 1;
  }
  int panelHeight = panel->buttonsCount * BUTTON_HEIGHT +
                    (panel->buttonsCount * 3) * BUTTON_SPACING;

  panel->position =
      (SDL_Rect){20, (HEIGHT - panelHeight - 30), 300, panelHeight};
  return 0;
}

void RenderPanel(Res *res, struct Panel *panel) {
  if (panel->isActive) {
    SDL_RenderCopy(res->renderer, panel->background, 0, &panel->position);
  }
}
void CleanupPanel(struct Panel *panel) {
  SDL_DestroyTexture(panel->background);
  free(panel);
}
//====================================AudioFunctions===============================
// 1 click, 2 error, 3 spawn
void PlayAudio(Res *resources, int index) {
  switch (index) {
  case 1:
    Mix_PlayMusic(resources->clickSFX, 0);
    break;
  case 2:
    Mix_PlayMusic(resources->errorSFX, 0);
    break;
  case 3:
    Mix_PlayMusic(resources->spawnSFX, 0);
    break;
  default:
    break;
  }
}

//===================================InputFunctions===============================

int InitInputField(Res *resurces, struct InputField *InputField, SDL_Rect rect,
                   const char *textFont, char *text, const int textSize) {
  if (!InputField) {
    printf("Input field pointer not valid or not detected! \n");
    return -1;
  }

  InputField->font = TTF_OpenFont(textFont, textSize);
  if (!InputField->font) {
    printf("Failed to load font %s: %s\n", textFont, TTF_GetError());
    return -1;
  }

  SDL_Color white = {0, 0, 0, 255};
  InputField->textColor = white;
  InputField->position = rect;
  InputField->bgPos = (SDL_Rect){rect.x - RECT_PADDING, rect.y - RECT_PADDING,
                                 (rect.w * 2), (rect.h * 2)};

  InputField->input = strdup(text ? text : "");
  if (!InputField->input) {
    printf("Failed to allocate memory for input text\n");
    TTF_CloseFont(InputField->font);
    return -1;
  }

  SDL_Surface *surf = TTF_RenderText_Solid(
      InputField->font, strlen(InputField->input) > 0 ? InputField->input : " ",
      InputField->textColor);
  if (!surf) {
    printf("Failed to create text surface: %s\n", TTF_GetError());
    free(InputField->input);
    TTF_CloseFont(InputField->font);
    return -1;
  }

  InputField->textTexture =
      SDL_CreateTextureFromSurface(resurces->renderer, surf);
  SDL_FreeSurface(surf);

  if (!InputField->textTexture) {
    printf("Failed to create text texture: %s\n", SDL_GetError());
    free(InputField->input);
    TTF_CloseFont(InputField->font);
    return -1;
  }

  InputField->background =
      IMG_LoadTexture(resurces->renderer, "textures/frame.png");
  InputField->deselectedBG =
      IMG_LoadTexture(resurces->renderer, "textures/frameUnactive.png");

  if (!InputField->background || !InputField->deselectedBG) {
    printf("Error at loading input field background textures\n");
    if (InputField->textTexture)
      SDL_DestroyTexture(InputField->textTexture);
    if (InputField->background)
      SDL_DestroyTexture(InputField->background);
    if (InputField->deselectedBG)
      SDL_DestroyTexture(InputField->deselectedBG);
    free(InputField->input);
    TTF_CloseFont(InputField->font);
    return -1;
  }

  return 0;
}

void UpdateInput(Res *resources, struct InputField *inputField,
                 char *newInput) {
  if (!newInput) {
    newInput = "";
  }

  SDL_Surface *textSurface = TTF_RenderText_Solid(
      inputField->font, strlen(newInput) > 0 ? newInput : " ",
      inputField->textColor);
  if (!textSurface) {
    printf("Failed to create text surface: %s\n", TTF_GetError());
    return;
  }

  int textWidth = textSurface->w;
  int textHeight = textSurface->h;

  const int MIN_WIDTH = 35;
  const int TEXT_PADDING = 10;

  int newTextWidth = (textWidth < MIN_WIDTH - (TEXT_PADDING * 2))
                         ? MIN_WIDTH - (TEXT_PADDING * 2)
                         : textWidth;
  int newBgWidth = newTextWidth + (TEXT_PADDING * 2);

  inputField->position.w = newTextWidth;
  inputField->position.h = textHeight;
  inputField->bgPos.w = newBgWidth;
  inputField->bgPos.h = textHeight + (TEXT_PADDING);

  inputField->position.x = inputField->bgPos.x + TEXT_PADDING;
  inputField->position.y = inputField->bgPos.y + (TEXT_PADDING / 2);

  if (inputField->textTexture) {
    SDL_DestroyTexture(inputField->textTexture);
    inputField->textTexture = NULL;
  }

  inputField->textTexture =
      SDL_CreateTextureFromSurface(resources->renderer, textSurface);
  SDL_FreeSurface(textSurface);

  if (!inputField->textTexture) {
    printf("Failed to create text texture: %s\n", SDL_GetError());
    return;
  }

  if (inputField->active) {
    SDL_RenderCopy(resources->renderer, inputField->background, NULL,
                   &inputField->bgPos);
  } else {
    SDL_RenderCopy(resources->renderer, inputField->deselectedBG, NULL,
                   &inputField->bgPos);
  }

  SDL_RenderCopy(resources->renderer, inputField->textTexture, NULL,
                 &inputField->position);
}
int InitNode(Node_v *node, Res *resurces, char *text, SDL_Rect rect, int index,
             int value) {
  if (index > MAXNODES) {
    printf("Exceded max amount of nodes!\n");
    return 1;
  }

  node->texture = IMG_LoadTexture(resurces->renderer, "textures/NodeV.png");
  node->SelectedTexture =
      IMG_LoadTexture(resurces->renderer, "textures/NodeSelect.png");
  if (!node->texture || !node->SelectedTexture) {
    printf("Failed at loading textures, Error: %s\n", IMG_GetError());
    return 1;
  }

  node->rect = rect;
  node->font = TTF_OpenFont(FONT_PATH, 34);
  if (!node->font) {
    printf("Failed to load font: %s\n", TTF_GetError());
    return 1;
  }

  SDL_Surface *surf = TTF_RenderText_Solid(
      node->font, strlen(text) ? text : " ", (SDL_Color){0, 0, 0, 255});
  if (!surf) {
    printf("Failed to create index text surface: %s\n", TTF_GetError());
    return 1;
  }
  node->textTexture = SDL_CreateTextureFromSurface(resurces->renderer, surf);
  SDL_FreeSurface(surf);

  SDL_Rect textRect = {rect.x, rect.y - 40, rect.w, 50};
  node->textRect = textRect;

  char content[20];
  sprintf(content, "%d", value);

  SDL_Surface *contSurf =
      TTF_RenderText_Solid(node->font, content, (SDL_Color){0, 0, 0, 255});
  if (!contSurf) {
    printf("Failed to create value text surface: %s\n", TTF_GetError());
    return 1;
  }

  int contentWidth = contSurf->w;
  int contentHeight = contSurf->h;

  node->contentTextTexture =
      SDL_CreateTextureFromSurface(resurces->renderer, contSurf);
  SDL_FreeSurface(contSurf);

  SDL_Rect contTextRect = {rect.x + (rect.w - contentWidth) / 2,
                           rect.y + (rect.h - contentHeight) / 2, contentWidth,
                           contentHeight};
  node->contentTextRect = contTextRect;

  node->position = (Vector2){rect.x, rect.y};
  node->next = NULL;
  node->isMoving = false;
  node->isSelected = false;

  if (!node->textTexture || !node->contentTextTexture) {
    printf("Failed at loading text textures, Error: %s\n", SDL_GetError());
    return 1;
  }

  return 0;
}

void UpdateList(Res *resources, Node_v *listHead) {
  if (!listHead)
    return;
  Node_v *current = listHead;
  Node_v *previous = listHead;
  int availableWidth = WIDTH;
  int newNodeWidth = NODE_WIDTH - ((resources->nodesNumber - 5) * 6);
  int newNodeHeight = NODE_HEIGHT - ((resources->nodesNumber - 5) * 3);
  int newSpacing = (availableWidth - (resources->nodesNumber * newNodeWidth)) /
                   (resources->nodesNumber + 1);
  if (newNodeWidth < 50)
    newNodeWidth = 50;
  if (newNodeHeight < 50)
    newNodeHeight = 50;
  if (newSpacing < 5)
    newSpacing = 5;

  current = listHead;
  int index = 0;
  // update positions and sizes
  while (current && index < resources->nodesNumber) {
    current->rect.w = newNodeWidth;
    current->rect.h = newNodeHeight;
    int reversedIndex = resources->nodesNumber - 1 - index;
    current->position.x =
        newSpacing + (reversedIndex * (newNodeWidth + newSpacing));
    current->position.y = current->position.y;
    current->textRect.w = newNodeWidth - (newNodeWidth / 2);
    current->textRect.h = newNodeHeight - (newNodeHeight / 1.5);
    current->textRect.x = current->position.x;
    current->textRect.y = current->position.y - 35;
    current->contentTextRect.x =
        current->position.x +
        (float)(newNodeWidth - current->contentTextRect.w) / 2;
    current->contentTextRect.y =
        current->position.y +
        (float)(newNodeHeight - current->contentTextRect.h) / 2;
    current = current->next;
    index++;
  }

  // reset for second pass
  current = listHead;
  previous = NULL;
  Uint32 color;
  while (current) {
    // arrow positioning and rendering
    if (previous != NULL) {
      current->nodePointerArrow.endX = previous->position.x;
      current->nodePointerArrow.endY =
          previous->position.y + (float)newNodeHeight / 2;
      current->nodePointerArrow.startX = current->position.x + newNodeHeight;
      current->nodePointerArrow.startY =
          (current->position.y + (float)newNodeHeight / 2);
      if (current->isSelected) {
        color = 0xFF0000FF;
      } else {
        color = 0x000000FF;
      }

      draw_arrow(resources->renderer, current->nodePointerArrow.startX,
                 current->nodePointerArrow.startY,
                 current->nodePointerArrow.endX, current->nodePointerArrow.endY,
                 6, 14, color);
    }

    if (current != NULL && current->isMoving) {
      Vector2 targetPos = {current->position.x, 200};
      current->position =
          V2Lerp(current->position, targetPos, 40, &current->isMoving);
    }

    current->rect.x = current->position.x;
    current->rect.y = current->position.y;
    current->textRect.x = current->position.x + (int)(newNodeWidth / 4);
    current->textRect.y = current->position.y - (int)(newNodeHeight / 3);
    current->contentTextRect.x =
        current->position.x +
        (float)(current->rect.w - current->contentTextRect.w) / 2;
    current->contentTextRect.y =
        current->position.y +
        (float)(current->rect.h - current->contentTextRect.h) / 2;

    // Render the node
    if (current->texture && current->textTexture &&
        current->contentTextTexture) {
      if (!current->isSelected) {
        SDL_RenderCopy(resources->renderer, current->texture, NULL,
                       &current->rect);
      } else {
        SDL_RenderCopy(resources->renderer, current->SelectedTexture, NULL,
                       &current->rect);
      }
      SDL_RenderCopy(resources->renderer, current->textTexture, NULL,
                     &current->textRect);
      SDL_RenderCopy(resources->renderer, current->contentTextTexture, NULL,
                     &current->contentTextRect);
    }

    previous = current;
    current = current->next;
  }
}
void FreeNodesInfo(Node_v **listHead) {
  if (!listHead || !*listHead) {
    return;
  }

  Node_v *current = *listHead;
  while (current) {
    Node_v *temp = current;
    current = current->next;

    if (temp->textTexture) {
      SDL_DestroyTexture(temp->textTexture);
    }
    if (temp->contentTextTexture) {
      SDL_DestroyTexture(temp->contentTextTexture);
    }
    if (temp->texture) {
      SDL_DestroyTexture(temp->texture);
    }
    if (temp->SelectedTexture) {
      SDL_DestroyTexture(temp->SelectedTexture);
    }
    if (temp->font) {
      TTF_CloseFont(temp->font);
    }
    free(temp);
  }
  *listHead = NULL;
}
Node_v *AddNodeToList(Res *resources, Node_v **listHead, int value) {
  Node_v *nodeVis = malloc(sizeof(Node_v));
  if (!nodeVis) {
    printf("Failed to allocate memory for new node\n");
    return NULL;
  }

  int baseX = 300 + resources->nodesNumber * 180;
  int baseY = 250;

  int randomYOffset = RandomInt(-100, 100);
  int finalY = baseY + randomYOffset;

  if (finalY < 50)
    finalY = 50;
  if (finalY > HEIGHT - NODE_HEIGHT - 50)
    finalY = HEIGHT - NODE_HEIGHT - 50;

  *nodeVis = (Node_v){
      .texture = NULL,
      .textTexture = NULL,
      .nodeIndex = resources->nodesNumber + 1,
      .font = NULL,
      .textRect = {0},
      .rect = (SDL_Rect){baseX, finalY, NODE_WIDTH, NODE_HEIGHT},
      .next = NULL,
  };

  resources->nodesNumber++;

  char name[20];
  sprintf(name, "Node %d", nodeVis->nodeIndex);

  if (InitNode(nodeVis, resources, name, nodeVis->rect, nodeVis->nodeIndex,
               value)) {
    free(nodeVis);
    resources->nodesNumber--;
    return NULL;
  }

  nodeVis->textRect.x =
      nodeVis->rect.x + (nodeVis->rect.w - nodeVis->textRect.w) / 2;
  nodeVis->textRect.y =
      nodeVis->rect.y + (nodeVis->rect.h - nodeVis->textRect.h);

  nodeVis->position = (Vector2){nodeVis->rect.x, nodeVis->rect.y};
  nodeVis->isMoving = false;
  nodeVis->next = *listHead;
  *listHead = nodeVis;

  return nodeVis;
}

void CheckList(Node_v *node, int mouseX, int mouseY, bool isClick) {
  Node_v *temp = node;

  if (isClick) {
    while (temp) {
      temp->isSelected = false;
      temp = temp->next;
    }
    temp = node;
  }

  while (temp) {

    if (IsInsideBox(temp->rect.x, temp->rect.y, temp->rect.w, temp->rect.h,
                    mouseX, mouseY)) {
      if (isClick) {
        temp->isSelected = true;
        temp->isMoving = true;
        break;
      }
    }
    temp = temp->next;
  }
}
int InitPopPanel(Res *res, struct PopupPanel *panel, int x, int y, int width,
                 int height) {
  if (!panel) {
    printf("PopupPanel pointer is NULL!\n");
    return 1;
  }

  panel->position = (SDL_Rect){x, y, width, height};
  panel->Vposition = (Vector2){x, y};
  panel->isActive = 0;

  panel->background = IMG_LoadTexture(res->renderer, "textures/filePanel.png");
  if (!panel->background) {
    printf("Failed at loading popup panel texture, Error: %s\n",
           IMG_GetError());
    return 1;
  }

  panel->inputField = malloc(sizeof(struct InputField));
  if (!panel->inputField) {
    printf("Failed to allocate memory for input field!\n");
    return 1;
  }

  SDL_Rect inputRect = {x + BUTTON_SPACING * 2, y + BUTTON_SPACING * 2,
                        width - (BUTTON_SPACING * 4), 60};
  panel->input = strdup("Type Here");
  if (InitInputField(res, panel->inputField, inputRect, FONT_PATH, panel->input,
                     32) != 0) {
    printf("Failed to initialize input field!\n");
    free(panel->inputField);
    return 1;
  }

  panel->addButton = (struct Button){
      .background = NULL,
      .selectedBG = NULL,
      .isActive = 1,
      .isCLicked = 0,
      .position = {x + BUTTON_SPACING * 2,
                   y + height - BUTTON_HEIGHT - BUTTON_SPACING * 2,
                   width - (BUTTON_SPACING * 4), BUTTON_HEIGHT},
      .textColor = {0, 0, 0, 255},
      .text = NULL,
      .font = NULL,
      .textTexture = NULL,
      .textRect = {0}};

  if (InitButton(res, &panel->addButton, "textures/buttonReady.png",
                 "textures/button.png") != 0) {
    printf("Failed to initialize add button!\n");
    return 1;
  }

  InitButtonText(res, &panel->addButton, "Add Value", 28, FONT_PATH);

  return 0;
}

void RenderPopPanel(Res *res, struct PopupPanel *panel) {
  if (!panel || !panel->isActive) {
    return;
  }
  SDL_RenderCopy(res->renderer, panel->background, 0, &panel->position);
  if (panel->inputField && panel->input) {
    UpdateInput(res, panel->inputField, panel->input);
  }
  DrawButton(res, &panel->addButton);
}

void CleanupPopPanel(struct PopupPanel *panel) {
  if (!panel) {
    return;
  }

  if (panel->background) {
    SDL_DestroyTexture(panel->background);
    panel->background = NULL;
  }

  if (panel->inputField) {
    if (panel->inputField->textTexture) {
      SDL_DestroyTexture(panel->inputField->textTexture);
    }
    if (panel->inputField->background) {
      SDL_DestroyTexture(panel->inputField->background);
    }
    if (panel->inputField->deselectedBG) {
      SDL_DestroyTexture(panel->inputField->deselectedBG);
    }
    if (panel->inputField->font) {
      TTF_CloseFont(panel->inputField->font);
    }

    if (panel->inputField->input) {
      free(panel->inputField->input);
      panel->inputField->input = NULL;
    }

    free(panel->inputField);
    panel->inputField = NULL;
  }

  if (panel->input) {
    free(panel->input);
    panel->input = NULL;
  }

  CleanUpButton(&panel->addButton);
}
void ShowPopPanel(struct PopupPanel *panel) {
  if (panel) {
    panel->isActive = 1;
    if (panel->inputField) {
      panel->inputField->active = 1;
    }
  }
}

void HidePopPanel(struct PopupPanel *panel) {
  if (panel) {
    panel->isActive = 0;
    if (panel->inputField) {
      panel->inputField->input = strdup("");
      panel->inputField->active = 0;
    }
  }
}

void HandlePopPanelInput(struct PopupPanel *panel, char *newInput) {
  if (!panel || !newInput) {
    return;
  }

  if (panel->input) {
    free(panel->input);
    panel->input = NULL;
  }

  panel->input = strdup(newInput);

  if (panel->inputField) {
    if (panel->inputField->input) {
      free(panel->inputField->input);
      panel->inputField->input = NULL;
    }
    panel->inputField->input = strdup(newInput);
  }
}
bool IsAddButtonClicked(struct PopupPanel *panel, int mouseX, int mouseY) {
  if (!panel || !panel->isActive) {
    return false;
  }

  return IsInsideBox(panel->addButton.position.x, panel->addButton.position.y,
                     panel->addButton.position.w, panel->addButton.position.h,
                     mouseX, mouseY);
}

bool IsInputFieldClicked(struct PopupPanel *panel, int mouseX, int mouseY) {
  if (!panel || !panel->isActive || !panel->inputField) {
    return false;
  }

  return IsInsideBox(panel->position.x, panel->position.y, panel->position.w,
                     panel->position.h, mouseX, mouseY);
}

void ScrollBg(Res *resources, double deltaTime, float scrollSpeed) {
  resources->bgOffsetX += scrollSpeed * deltaTime;

  if (resources->bgOffsetX >= resources->bgW) {
    resources->bgOffsetX -= resources->bgW;
  }

  float scaleX = (float)WIDTH / resources->bgW;
  float scaleY = (float)HEIGHT / resources->bgH;

  int leftWidth = resources->bgW - (int)resources->bgOffsetX;
  int scaledLeftWidth = (int)(leftWidth * scaleX);

  SDL_Rect src1 = {(int)resources->bgOffsetX, 0, leftWidth, resources->bgH};
  SDL_Rect dest1 = {0, 0, scaledLeftWidth, HEIGHT};

  if ((int)resources->bgOffsetX > 0) {
    int rightWidth = (int)resources->bgOffsetX;
    int scaledRightWidth = (int)(rightWidth * scaleX);

    SDL_Rect src2 = {0, 0, rightWidth, resources->bgH};
    SDL_Rect dest2 = {scaledLeftWidth, 0, scaledRightWidth, HEIGHT};
    SDL_RenderCopy(resources->renderer, resources->bgTexture, &src2, &dest2);
  }

  SDL_RenderCopy(resources->renderer, resources->bgTexture, &src1, &dest1);
}

void draw_arrow(SDL_Renderer *renderer, int ax, int ay, int bx, int by,
                int thickness, int arrowhead_size, Uint32 color) {
  double dx = bx - ax;
  double dy = by - ay;
  double length = sqrt(dx * dx + dy * dy);

  if (length == 0)
    return;

  double unit_x = dx / length;
  double unit_y = dy / length;

  int shaft_end_x = (int)(bx - unit_x * arrowhead_size * 0.7);
  int shaft_end_y = (int)(by - unit_y * arrowhead_size * 0.7);

  thickLineRGBA(renderer, ax, ay, shaft_end_x, shaft_end_y, thickness,
                (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF,
                color & 0xFF);

  double perp_x = -unit_y;
  double perp_y = unit_x;

  Sint16 arrowhead_x[3] = {
      (Sint16)bx,
      (Sint16)(bx - unit_x * arrowhead_size + perp_x * arrowhead_size * 0.5),
      (Sint16)(bx - unit_x * arrowhead_size - perp_x * arrowhead_size * 0.5)};

  Sint16 arrowhead_y[3] = {
      (Sint16)by,
      (Sint16)(by - unit_y * arrowhead_size + perp_y * arrowhead_size * 0.5),
      (Sint16)(by - unit_y * arrowhead_size - perp_y * arrowhead_size * 0.5)};

  filledPolygonRGBA(renderer, arrowhead_x, arrowhead_y, 3, (color >> 24) & 0xFF,
                    (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}
