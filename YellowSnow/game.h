#ifdef GAME_H
#define GAME_H
#include "main.h"

struct Game game {
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Window *window;
} bool Initializare(struct Game *game);
void Quit(stuct Game *game);
#endif // GAME_H
#define GAME_H
