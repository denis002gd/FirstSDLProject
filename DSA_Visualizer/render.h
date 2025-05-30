#ifndef RENDER_H
#define RENDER_H
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#define HEIGHT 1000
#define WIDTH 1600
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 90
#define BUTTON_SPACING 10
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
#define REFRESHRATE 16
// h
#include <SDL2/SDL.h>

typedef struct Resources {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Color *bgColor;
  Mix_Music *clickSFX;
  Mix_Music *errorSFX;
} Res;

struct Button {
  SDL_Texture *background;
  SDL_Texture *selectedBG;
  int isCLicked; // 0 false 1 true
  int isActive;
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
  int isActive;
  int panelIndex;
  int buttonsCount;
  struct Button buttons[];
};
int InitProgram(Res *resources);
void CleanupProgram(Res *resources);

void PlayAudio(Res *resources, int index);
int InitButton(Res *resources, struct Button *button, char *path,
               char *secondPath);
void DrawButton(Res *resources, struct Button *button);
void InitButtonText(Res *res, struct Button *button, const char *text,
                    int textSize, const char *font);
void CleanUpButton(struct Button *button);

int InitPanel(Res *res, struct Panel *panel);

void RenderPanel(Res *res, struct Panel *panel);
void CleanupPanel(struct Panel *panel);
#endif // !DEBUG:
