#include "render.h"
#include "list.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
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
  if (resources->clickSFX == NULL || resources->errorSFX == NULL) {
    fprintf(stdout, "SDL_mixer did not find audio file! Error: %s\n",
            Mix_GetError());
  }
  resources->renderer = SDL_CreateRenderer(resources->window, -1, 0);
  if (!resources->renderer) {
    fprintf(stderr, "Error on renderer initialization, Error: %s\n",
            SDL_GetError());
    return 1;
  }
  // return 0 on success
  return 0;
}

void CleanupProgram(Res *resources) {
  SDL_DestroyRenderer(resources->renderer);
  SDL_DestroyWindow(resources->window);
  Mix_FreeMusic(resources->clickSFX);
  Mix_FreeMusic(resources->errorSFX);
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
// 1 click, 2 error
void PlayAudio(Res *resources, int index) {
  switch (index) {
  case 1:
    Mix_PlayMusic(resources->clickSFX, 0);
    break;
  case 2:
    Mix_PlayMusic(resources->errorSFX, 0);
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
  // just white
  SDL_Color white = {0, 0, 0, 255};
  InputField->textColor = white;
  InputField->position = rect;
  InputField->bgPos = (SDL_Rect){rect.x - RECT_PADDING, rect.y - RECT_PADDING,
                                 (rect.w * 2), (rect.h * 2)};
  InputField->input = strdup(text);
  SDL_Surface *surf = TTF_RenderText_Solid(InputField->font, InputField->input,
                                           InputField->textColor);
  InputField->textTexture =
      SDL_CreateTextureFromSurface(resurces->renderer, surf);

  InputField->background =
      IMG_LoadTexture(resurces->renderer, "textures/frame.png");
  InputField->deselectedBG =
      IMG_LoadTexture(resurces->renderer, "textures/frameUnactive.png");
  if (InputField->background == NULL || InputField->deselectedBG == NULL) {
    printf("Error at loading input field background \n");
  }
  SDL_FreeSurface(surf);
  if (InputField->textTexture == NULL) {
    fprintf(stdout, "Input field texture error: %s \n", SDL_GetError());
    return -1;
  }
  return 0;
}
// TODO: make the bg rect dynamic based on the input size

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
int InitNode(Node_v *node, Res *resurces, char *text, SDL_Rect rect,
             int index) {
  if (index > MAXNODES) {
    printf("Excceded max amount of nodes!\n");
    return 1;
  }
  node->texture = IMG_LoadTexture(resurces->renderer, "textures/NodeV.png");

  if (!node->texture) {
    printf("Failed at loading textures, Error: %s\n", IMG_GetError());
    return 1;
  }
  node->rect = rect;
  node->font = TTF_OpenFont(FONT_PATH, 28);
  SDL_Surface *surf = TTF_RenderText_Solid(
      node->font, strlen(text) ? text : " ", (SDL_Color){0, 0, 0, 255});
  node->textTexture = SDL_CreateTextureFromSurface(resurces->renderer, surf);
  SDL_FreeSurface(surf);
  SDL_Rect textRect = {rect.x + (rect.w / 4), rect.y - (rect.h / 3), rect.w / 2,
                       rect.h / 3};
  node->textRect = textRect;
  node->position = (Vector2){rect.x, rect.y};
  node->next = NULL;
  if (!node->textTexture) {
    printf("Failed at loading text textures, Error: %s\n", SDL_GetError());
    return 1;
  }

  return 0;
}
// this function rescales the nodes based on number
void UpdateList(Res *resources, Node_v *listHead) {
  if (!listHead)
    return;

  Node_v *current = listHead;

  int availableWidth = WIDTH;
  int newNodeWidth = NODE_WIDTH - ((resources->nodesNumber - 5) * 6);
  int newNodeHeight = NODE_HEIGHT - ((resources->nodesNumber - 5) * 3);
  int newSpacing = (availableWidth - (resources->nodesNumber * newNodeWidth)) /
                   (resources->nodesNumber + 1);

  current = listHead;
  int index = 0;
  while (current && index < resources->nodesNumber) {
    current->rect.w = newNodeWidth;
    current->rect.h = newNodeHeight;

    int reversedIndex = resources->nodesNumber - 1 - index;
    current->rect.x =
        newSpacing + (reversedIndex * (newNodeWidth + newSpacing));
    current->rect.y = current->position.y; // Keep original Y from spawn

    current->textRect.w = NODE_WIDTH - ((resources->nodesNumber) * 6);
    current->textRect.h = (NODE_HEIGHT - 40) - ((resources->nodesNumber) * 2);

    current->textRect.x =
        current->rect.x + (current->rect.w - current->textRect.w) / 2;
    current->textRect.y =
        (current->rect.y + (current->rect.h - current->textRect.h) / 2) -
        current->rect.h / 1.5;

    current = current->next;
    index++;
  }

  current = listHead;
  while (current) {
    if (current->texture && current->textTexture) {
      SDL_RenderCopy(resources->renderer, current->texture, NULL,
                     &current->rect);
      SDL_RenderCopy(resources->renderer, current->textTexture, NULL,
                     &current->textRect);
    }
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
    if (temp->texture) {
      SDL_DestroyTexture(temp->texture);
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

  if (InitNode(nodeVis, resources, name, nodeVis->rect, nodeVis->nodeIndex) !=
      0) {
    free(nodeVis);
    resources->nodesNumber--;
    return NULL;
  }

  nodeVis->textRect.x =
      nodeVis->rect.x + (nodeVis->rect.w - nodeVis->textRect.w) / 2;
  nodeVis->textRect.y =
      nodeVis->rect.y + (nodeVis->rect.h - nodeVis->textRect.h) / 2;

  nodeVis->position = (Vector2){nodeVis->rect.x, nodeVis->rect.y};

  nodeVis->next = *listHead;
  *listHead = nodeVis;

  return nodeVis;
}
