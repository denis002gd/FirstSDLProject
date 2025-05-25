#include "render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

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
  resources->renderer = SDL_CreateRenderer(resources->window, -1, 0);
  if (!resources->renderer) {
    fprintf(stderr, "Error on renderer initialization, Error: %s\n",
            SDL_GetError());
    return 1;
  }
  // return 0 on success
  return 0;
}

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
int InitPanel(Res *res, struct Panel *panel) {
  panel->background = IMG_LoadTexture(res->renderer, "textures/filePanel.png");
  if (!panel->background) {
    fprintf(stdout, "Texture loading error, Error log: %s\n", IMG_GetError());
    return 1;
  }
  panel->position = (SDL_Rect){20, 500, 450, 500};
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
void CleanupProgram(Res *resources) {
  SDL_DestroyRenderer(resources->renderer);
  SDL_DestroyWindow(resources->window);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
}
