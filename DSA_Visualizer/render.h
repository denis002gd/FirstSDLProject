#ifndef RENDER_H
#define RENDER_H
#define HEIGHT 700
#define WIDTH 900
// forward declaration (ts magic fr)
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Resources {
  SDL_Window *window;
  SDL_Renderer *renderer;
} Res;

int InitProgram(Res *resources);
void CleanupProgram(Res *resources);

#endif // !DEBUG:
