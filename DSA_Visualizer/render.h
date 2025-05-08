#ifndef RENDER_H
#define RENDER_H
#define HEIGHT 700
#define WIDTH 900
#define REFRESHRATE 16
// forward declaration (ts magic fr)
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Resources {
  SDL_Window *window;
  SDL_Renderer *renderer;
} Res;

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Rect SDL_Rect;

// TODO: add another texture for selected button state

struct Button {
  SDL_Texture *background;
  SDL_Rect *position;
};

int InitProgram(Res *resources);
int InitButton(Res *resources, struct Button *button, int width, int height);
void DrawButton(Res *resources, struct Button *button);
void CleanUpButton(struct Button *button);
void CleanupProgram(Res *resources);

#endif // !DEBUG:
