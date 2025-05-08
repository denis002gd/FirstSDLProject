#ifndef RENDER_H
#define RENDER_H
#define HEIGHT 700
#define WIDTH 900
#define REFRESHRATE 16

#include <SDL2/SDL.h>

typedef struct Resources {
  SDL_Window *window;
  SDL_Renderer *renderer;
} Res;

// TODO: add another texture for selected button state

struct Button {
  SDL_Texture *background;
  SDL_Rect position;
};

int InitProgram(Res *resources);
int InitButton(Res *resources, struct Button *button, char *path);
void DrawButton(Res *resources, struct Button *button);
void CleanUpButton(struct Button *button);
void CleanupProgram(Res *resources);

#endif // !DEBUG:
