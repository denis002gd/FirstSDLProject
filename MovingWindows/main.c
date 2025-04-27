#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PL {
  SDL_Rect playerRect;
  SDL_Texture **playerSprites; // Array texturi pentru fiecare fereasta
  // pozitia absoluta a playerului (globala)
  float screenX;
  float screenY;
  int numSprites; // Num de sprite-uri pe fereastra
} player;

typedef struct NewWin {
  SDL_Window *window;
  SDL_Renderer *renderer;
  char *name;
  // o sa schimb cu un texture cand o sa am chef
  SDL_Color bgColor;
  struct NewWin *next;
} Window_t;

Window_t *windowList = NULL; // Capul
Window_t *movableWindow = NULL;
player Player;
int running = 1;

int initialize();
void cleanup();
void gameLoop();
Window_t *CreateNewWindow(Window_t **Head, char *Name, int x, int y, int w,
                          int h);
void ClearAllWindows(Window_t *Head);
void RenderAllWindows();
int CountWindows(Window_t *Head);

int main(int argc, char *argv[]) {
  if (!initialize()) {
    cleanup();
    return 1;
  }

  gameLoop();

  cleanup();

  return 0;
}

int CountWindows(Window_t *Head) {
  int count = 0;
  Window_t *current = Head;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

int initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
    return 0;
  }

  int img_flags = IMG_INIT_PNG;
  if ((IMG_Init(img_flags) & img_flags) != img_flags) {
    fprintf(stderr, "SDL_image failed to initialize: %s\n", IMG_GetError());
    return 0;
  }

  movableWindow =
      CreateNewWindow(&windowList, "POV: you are the POV",
                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 250, 200);
  if (!movableWindow) {
    return 0;
  }
  movableWindow->bgColor = (SDL_Color){255, 255, 255, 255};

  int baseX = 100;
  int baseY = 500;
  int width = 200;
  int height = 150;
  int spacing = 20;

  // astea-s de test
  for (int i = 0; i < 5; i++) {
    char windowName[32];
    sprintf(windowName, "Test Window %d", i + 1);
    Window_t *testWindow =
        CreateNewWindow(&windowList, windowName, baseX + i * (width + spacing),
                        baseY, width, height);
    if (!testWindow) {
      fprintf(stderr, "Failed to create test window %d\n", i + 1);
      continue;
    }
    testWindow->bgColor = (SDL_Color){255, 255, 255, 255}; // White background
  }

  Player.playerRect = (SDL_Rect){0, 0, 50, 50};

  int windowCount = CountWindows(windowList);
  Player.numSprites = windowCount;
  Player.playerSprites =
      (SDL_Texture **)malloc(sizeof(SDL_Texture *) * windowCount);

  if (!Player.playerSprites) {
    fprintf(stderr, "Failed to allocate memory for player sprites\n");
    return 0;
  }

  int windowX, windowY;
  SDL_GetWindowPosition(movableWindow->window, &windowX, &windowY);
  Player.screenX = windowX + 100.0f; // Start in a visible position
  Player.screenY = windowY + 100.0f;

  // Load la texturi pentru ferestre separat
  int i = 0;
  Window_t *current = windowList;
  while (current != NULL && i < windowCount) {
    Player.playerSprites[i] =
        IMG_LoadTexture(current->renderer, "art/blackCircle.png");
    if (!Player.playerSprites[i]) {
      fprintf(stderr, "Failed to load player sprite for window %d: %s\n", i,
              IMG_GetError());
      // Continue anyway, dar nu va arata playerul
    }
    current = current->next;
    i++;
  }

  return 1;
}

Window_t *CreateNewWindow(Window_t **Head, char *Name, int x, int y, int w,
                          int h) {
  Window_t *newWindow = (Window_t *)calloc(1, sizeof(Window_t));
  if (newWindow == NULL) {
    fprintf(stderr, "Failed to create new window\n");
    return NULL;
  }

  newWindow->name = malloc(strlen(Name) + 1);
  if (newWindow->name == NULL) {
    fprintf(stderr, "Failed to allocate memory for window name\n");
    free(newWindow);
    return NULL;
  }
  strcpy(newWindow->name, Name);

  newWindow->window =
      SDL_CreateWindow(newWindow->name, x, y, w, h, SDL_WINDOW_SHOWN);
  if (!newWindow->window) {
    fprintf(stderr, "Window could not be created: %s\n", SDL_GetError());
    free(newWindow->name);
    free(newWindow);
    return NULL;
  }

  newWindow->renderer =
      SDL_CreateRenderer(newWindow->window, -1, SDL_RENDERER_ACCELERATED);
  if (!newWindow->renderer) {
    fprintf(stderr, "Renderer could not be created: %s\n", SDL_GetError());
    SDL_DestroyWindow(newWindow->window);
    free(newWindow->name);
    free(newWindow);
    return NULL;
  }

  newWindow->next = *Head;
  *Head = newWindow;
  return newWindow;
}

void ClearAllWindows(Window_t *Head) {
  Window_t *current = Head;
  while (current != NULL) {
    Window_t *temp = current->next;

    if (current->renderer != NULL) {
      SDL_DestroyRenderer(current->renderer);
    }

    if (current->window != NULL) {
      SDL_DestroyWindow(current->window);
    }

    if (current->name != NULL) {
      free(current->name);
    }

    free(current);
    current = temp;
  }
}

void RenderAllWindows() {
  Window_t *current = windowList;
  int windowIndex = 0;

  while (current != NULL) {
    int windowX, windowY;
    SDL_GetWindowPosition(current->window, &windowX, &windowY);

    // Calcularea pozitiei playerului relativ cu fereastra asta
    SDL_Rect playerRectInWindow = Player.playerRect;
    playerRectInWindow.x = (int)Player.screenX - windowX;
    playerRectInWindow.y = (int)Player.screenY - windowY;

    SDL_SetRenderDrawColor(current->renderer, current->bgColor.r,
                           current->bgColor.g, current->bgColor.b,
                           current->bgColor.a);
    SDL_RenderClear(current->renderer);

    // Render la player daca exista si se afla in fereastra
    if (windowIndex < Player.numSprites &&
        Player.playerSprites[windowIndex] != NULL) {
      int windowWidth, windowHeight;
      SDL_GetWindowSize(current->window, &windowWidth, &windowHeight);

      if (playerRectInWindow.x < windowWidth &&
          playerRectInWindow.x + playerRectInWindow.w > 0 &&
          playerRectInWindow.y < windowHeight &&
          playerRectInWindow.y + playerRectInWindow.h > 0) {
        SDL_RenderCopy(current->renderer, Player.playerSprites[windowIndex],
                       NULL, &playerRectInWindow);
      }
    }

    SDL_RenderPresent(current->renderer);

    current = current->next;
    windowIndex++;
  }
}

void cleanup() {
  if (Player.playerSprites != NULL) {
    for (int i = 0; i < Player.numSprites; i++) {
      if (Player.playerSprites[i] != NULL) {
        SDL_DestroyTexture(Player.playerSprites[i]);
      }
    }
    free(Player.playerSprites);
    Player.playerSprites = NULL;
  }

  ClearAllWindows(windowList);
  windowList = NULL;
  movableWindow = NULL;

  IMG_Quit();
  SDL_Quit();
}

void gameLoop() {
  SDL_Event event;
  const float speed = 400.0f; // px pe secunda
  float windowX, windowY;
  int intWindowX, intWindowY;
  Uint32 lastTime = SDL_GetTicks(); // Milisecunde

  SDL_GetWindowPosition(movableWindow->window, &intWindowX, &intWindowY);
  windowX = (float)intWindowX;
  windowY = (float)intWindowY;

  while (running) {
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = 0; // Exit cand pesi pe ESC
        }
      }
    }

    float moveAmount = speed * deltaTime;
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);

    // controlezi fereastra cu WASD
    if (keyState[SDL_SCANCODE_W]) {
      windowY -= moveAmount;
    }
    if (keyState[SDL_SCANCODE_S]) {
      windowY += moveAmount;
    }
    if (keyState[SDL_SCANCODE_A]) {
      windowX -= moveAmount;
    }
    if (keyState[SDL_SCANCODE_D]) {
      windowX += moveAmount;
    }
    // controlezi playerul cu sageti
    if (keyState[SDL_SCANCODE_UP]) {
      Player.screenY -= moveAmount;
    }
    if (keyState[SDL_SCANCODE_DOWN]) {
      Player.screenY += moveAmount;
    }
    if (keyState[SDL_SCANCODE_LEFT]) {
      Player.screenX -= moveAmount;
    }
    if (keyState[SDL_SCANCODE_RIGHT]) {
      Player.screenX += moveAmount;
    }

    SDL_SetWindowPosition(movableWindow->window, (int)windowX, (int)windowY);

    RenderAllWindows();

    SDL_Delay(1);
  }
}
