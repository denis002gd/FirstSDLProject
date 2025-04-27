#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum tip {
  STATIC,
  DINAMIC,
  CONTROLAT,
};

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
  enum tip type;
  struct NewWin *next;
  // Movement properties
  int isMoving;          // Boolean for if window is currently moving
  int loopMovement;      // Boolean for continuous movement between points
  SDL_Point startPos;    // Starting position
  SDL_Point endPos;      // Ending position
  float moveSpeed;       // Movement speed (pixels per second)
  float currentProgress; // Progress between 0.0 and 1.0
  int moveDirection;     // 1 for start->end, -1 for end->start
  // Player texture for this window
  SDL_Texture *playerTexture;
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
int RandomInt(int min, int max);
void SetWindowMovement(Window_t *window, int startX, int startY, int endX,
                       int endY, float speed, int loopMovement);
void UpdateMovingWindows(float deltaTime);
int IsPlayerVisibleInAnyWindow();
void ShowGameOverWindow();
void LoadPlayerTextureForWindow(Window_t *window);

int main(int argc, char *argv[]) {
  srand(time(NULL));
  if (!initialize()) {
    cleanup();
    return 1;
  }

  gameLoop();

  cleanup();

  return 0;
}

int RandomInt(int min, int max) { return (rand() % max) + min; }

int CountWindows(Window_t *Head) {
  int count = 0;
  Window_t *current = Head;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

// Load player texture for a specific window
void LoadPlayerTextureForWindow(Window_t *window) {
  if (!window || !window->renderer)
    return;

  // Load player texture for this window
  window->playerTexture =
      IMG_LoadTexture(window->renderer, "art/blackCircle.png");
  if (!window->playerTexture) {
    fprintf(stderr, "Failed to load player texture: %s\n", IMG_GetError());
  }
}

// Set a window to move between two positions
void SetWindowMovement(Window_t *window, int startX, int startY, int endX,
                       int endY, float speed, int loopMovement) {
  if (!window)
    return;

  // Set initial position
  SDL_SetWindowPosition(window->window, startX, startY);

  // Set movement properties
  window->isMoving = 1;
  window->loopMovement = loopMovement;
  window->startPos.x = startX;
  window->startPos.y = startY;
  window->endPos.x = endX;
  window->endPos.y = endY;
  window->moveSpeed = speed;
  window->currentProgress = 0.0f;
  window->moveDirection = 1;
}

// Update the position of all moving windows
void UpdateMovingWindows(float deltaTime) {
  Window_t *current = windowList;

  while (current != NULL) {
    if (current->isMoving) {
      // Update progress
      current->currentProgress +=
          current->moveDirection * current->moveSpeed * deltaTime;

      // Check boundaries
      if (current->currentProgress >= 1.0f) {
        if (current->loopMovement) {
          // Reverse direction
          current->moveDirection = -1;
          current->currentProgress = 1.0f;
        } else {
          // Stop at destination
          current->currentProgress = 1.0f;
          current->isMoving = 0;
        }
      } else if (current->currentProgress <= 0.0f) {
        if (current->loopMovement) {
          // Reverse direction
          current->moveDirection = 1;
          current->currentProgress = 0.0f;
        } else {
          // Stop at start
          current->currentProgress = 0.0f;
          current->isMoving = 0;
        }
      }

      // Calculate new position with linear interpolation
      int newX = current->startPos.x +
                 (int)((current->endPos.x - current->startPos.x) *
                       current->currentProgress);
      int newY = current->startPos.y +
                 (int)((current->endPos.y - current->startPos.y) *
                       current->currentProgress);

      // Set new position
      SDL_SetWindowPosition(current->window, newX, newY);
    }
    current = current->next;
  }
}

int IsPlayerVisibleInAnyWindow() {
  Window_t *current = windowList;

  while (current != NULL) {
    int windowX, windowY, windowWidth, windowHeight;
    SDL_GetWindowPosition(current->window, &windowX, &windowY);
    SDL_GetWindowSize(current->window, &windowWidth, &windowHeight);

    // Calculate player position relative to this window
    int playerX = (int)Player.screenX - windowX;
    int playerY = (int)Player.screenY - windowY;

    // Check if player is visible in this window
    if (playerX >= 0 && playerX < windowWidth && playerY >= 0 &&
        playerY < windowHeight) {
      return 1; // Player is visible in at least one window
    }

    current = current->next;
  }

  return 0; // Player is not visible in any window
}

void ShowGameOverWindow() {
  Window_t *gameOverWindow =
      CreateNewWindow(&windowList, "Game Over!", SDL_WINDOWPOS_CENTERED,
                      SDL_WINDOWPOS_CENTERED, 400, 200);
  if (!gameOverWindow) {
    fprintf(stderr, "Failed to create game over window\n");
    return;
  }

  gameOverWindow->bgColor = (SDL_Color){255, 0, 0, 255}; // Red background

  // Render "Game Over" text
  SDL_SetRenderDrawColor(gameOverWindow->renderer, gameOverWindow->bgColor.r,
                         gameOverWindow->bgColor.g, gameOverWindow->bgColor.b,
                         gameOverWindow->bgColor.a);
  SDL_RenderClear(gameOverWindow->renderer);

  // We would need SDL_ttf to render text properly
  // For now we'll just render a red window
  SDL_RenderPresent(gameOverWindow->renderer);
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
  LoadPlayerTextureForWindow(movableWindow);

  int baseX = 100;
  int baseY = 500;
  int width = 200;
  int height = 150;

  // Initialize new window struct fields
  movableWindow->isMoving = 0;
  movableWindow->loopMovement = 0;
  movableWindow->moveSpeed = 0.0f;
  movableWindow->currentProgress = 0.0f;
  movableWindow->moveDirection = 1;

  // astea-s de test
  for (int i = 0; i < 8; i++) {
    width = RandomInt(50, 500);
    height = (int)(width * 0.8);
    baseX = RandomInt(0, 1920 - width);
    baseY = RandomInt(0, 1080 - height);
    char windowName[32];
    sprintf(windowName, "Test Window %d", i + 1);
    Window_t *testWindow =
        CreateNewWindow(&windowList, windowName, baseX, baseY, width, height);
    if (!testWindow) {
      fprintf(stderr, "Failed to create test window %d\n", i + 1);
      continue;
    }
    testWindow->bgColor =
        (SDL_Color){RandomInt(0, 255), RandomInt(0, 255), RandomInt(0, 255),
                    255}; // White background

    // Load player texture for this window
    LoadPlayerTextureForWindow(testWindow);

    // Initialize new window struct fields
    testWindow->isMoving = 0;
    testWindow->loopMovement = 0;
    testWindow->moveSpeed = 0.0f;
    testWindow->currentProgress = 0.0f;
    testWindow->moveDirection = 1;

    // Make some windows moving randomly
    if (i % 3 == 0) {
      int endX = RandomInt(0, 1920 - width);
      int endY = RandomInt(0, 1080 - height);
      SetWindowMovement(testWindow, baseX, baseY, endX, endY,
                        (float)RandomInt(1, 5) / 10.0f,
                        i % 2); // Random speed, alternate loop
    }
  }

  Player.playerRect = (SDL_Rect){0, 0, 50, 50};

  int windowX, windowY;
  SDL_GetWindowPosition(movableWindow->window, &windowX, &windowY);
  Player.screenX = windowX + 100.0f; // Start in a visible position
  Player.screenY = windowY + 100.0f;

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

  // Initialize movement properties
  newWindow->isMoving = 0;
  newWindow->loopMovement = 0;
  newWindow->startPos.x = x;
  newWindow->startPos.y = y;
  newWindow->endPos.x = x;
  newWindow->endPos.y = y;
  newWindow->moveSpeed = 0.0f;
  newWindow->currentProgress = 0.0f;
  newWindow->bgColor = (SDL_Color){255, 255, 255, 255};
  newWindow->moveDirection = 1;
  newWindow->playerTexture = NULL;

  // Load player texture for this window
  LoadPlayerTextureForWindow(newWindow);

  newWindow->next = *Head;
  *Head = newWindow;
  return newWindow;
}

void ClearAllWindows(Window_t *Head) {
  Window_t *current = Head;
  while (current != NULL) {
    Window_t *temp = current->next;

    if (current->playerTexture != NULL) {
      SDL_DestroyTexture(current->playerTexture);
    }

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
    if (current->playerTexture != NULL) {
      int windowWidth, windowHeight;
      SDL_GetWindowSize(current->window, &windowWidth, &windowHeight);

      if (playerRectInWindow.x < windowWidth &&
          playerRectInWindow.x + playerRectInWindow.w > 0 &&
          playerRectInWindow.y < windowHeight &&
          playerRectInWindow.y + playerRectInWindow.h > 0) {
        SDL_RenderCopy(current->renderer, current->playerTexture, NULL,
                       &playerRectInWindow);
      }
    }

    SDL_RenderPresent(current->renderer);

    current = current->next;
  }
}

void cleanup() {
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
  int gameOver = 0;

  SDL_GetWindowPosition(movableWindow->window, &intWindowX, &intWindowY);
  windowX = (float)intWindowX;
  windowY = (float)intWindowY;

  // Create some moving windows for example
  Window_t *movingWindow1 =
      CreateNewWindow(&windowList, "Moving Window 1", 100, 100, 300, 200);
  SetWindowMovement(movingWindow1, 100, 100, 500, 100, 0.5f,
                    1); // Horizontal loop

  Window_t *movingWindow2 =
      CreateNewWindow(&windowList, "Moving Window 2", 100, 400, 300, 200);
  SetWindowMovement(movingWindow2, 100, 400, 100, 700, 0.3f,
                    1); // Vertical loop

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
        // Add N key to create new window
        else if (event.key.keysym.sym == SDLK_n) {
          Window_t *newWindow =
              CreateNewWindow(&windowList, "Spawned Window", RandomInt(0, 1600),
                              RandomInt(0, 900), 300, 250);
          newWindow->bgColor = (SDL_Color){RandomInt(0, 255), RandomInt(0, 255),
                                           RandomInt(0, 255), 255};
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

    // Update moving windows
    UpdateMovingWindows(deltaTime);

    // Check if player is visible in any window
    if (!gameOver && !IsPlayerVisibleInAnyWindow()) {
      gameOver = 1;
      ShowGameOverWindow();
    }

    RenderAllWindows();

    SDL_Delay(1);
  }
}
