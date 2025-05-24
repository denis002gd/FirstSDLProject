#ifndef RENDER_H
#define RENDER_H
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#define HEIGHT 700
#define WIDTH 900
#define REFRESHRATE 16

#include <SDL2/SDL.h>

typedef struct Resources {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Color *bgColor;
} Res;

// TODO: add another texture for selected button state

struct Button {
  SDL_Texture *background;
  SDL_Texture *selectedBG;
  int isCLicked; // 0 false 1 true
  SDL_Rect position;

  SDL_Color textColor;
  char *text;
  TTF_Font *font;
  SDL_Texture *textTexture;
  SDL_Rect textRect;
};
struct Panel {
  SDL_Texture *background;
  SDL_Rect position;
};
int InitProgram(Res *resources);
int InitButton(Res *resources, struct Button *button, char *path,
               char *secondPath);
void DrawButton(Res *resources, struct Button *button);
void InitButtonText(Res *res, struct Button *button, char *text, int textSize,
                    char *font);
void CleanUpButton(struct Button *button);
void CleanupProgram(Res *resources);

#endif // !DEBUG:
