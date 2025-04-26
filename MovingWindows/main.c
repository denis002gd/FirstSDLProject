#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

typedef struct PL {
  SDL_Rect playerRect;
  SDL_Texture *playerSprite;
  int screenX; // Absolute screen position
  int screenY; // Absolute screen position
} player;

// Global variables
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
player Player;
int running = 1;

// Function prototypes
int initialize();
void cleanup();
void gameLoop();

int main(int argc, char *argv[]) {
  // Initialize systems and resources
  if (!initialize()) {
    cleanup();
    return 1;
  }

  // Run the game loop
  gameLoop();

  // Clean up resources
  cleanup();

  return 0;
}

int initialize() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
    return 0;
  }

  // Initialize SDL_image
  int img_flags = IMG_INIT_PNG;
  if ((IMG_Init(img_flags) & img_flags) != img_flags) {
    fprintf(stderr, "SDL_image failed to initialize: %s\n", IMG_GetError());
    return 0;
  }

  // Create window
  window = SDL_CreateWindow("POV: you are the POV", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Window could not be created: %s\n", SDL_GetError());
    return 0;
  }

  // Create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Renderer could not be created: %s\n", SDL_GetError());
    return 0;
  }

  // Initialize player
  Player.playerRect =
      (SDL_Rect){375, 275, 50, 50}; // Centered position with size 50x50

  // Get window position to initialize absolute screen position
  int windowX, windowY;
  SDL_GetWindowPosition(window, &windowX, &windowY);

  // Set player's absolute screen position (initial position on screen)
  Player.screenX = windowX + Player.playerRect.x;
  Player.screenY = windowY + Player.playerRect.y;

  // Load player sprite
  Player.playerSprite = IMG_LoadTexture(renderer, "art/blackCircle.png");
  if (!Player.playerSprite) {
    fprintf(stderr, "Failed to load player sprite: %s\n", IMG_GetError());
    return 0;
  }

  return 1; // Initialization successful
}

void cleanup() {
  if (Player.playerSprite != NULL) {
    SDL_DestroyTexture(Player.playerSprite);
    Player.playerSprite = NULL;
  }

  if (renderer != NULL) {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
  }

  if (window != NULL) {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  IMG_Quit();
  SDL_Quit();
}

void gameLoop() {
  SDL_Event event;
  const float speed = 300.0f; // Pixels per second
  float windowX, windowY;
  int intWindowX, intWindowY;
  Uint32 lastTime = SDL_GetTicks(); // Milliseconds

  SDL_GetWindowPosition(window, &intWindowX, &intWindowY);
  windowX = (float)intWindowX;
  windowY = (float)intWindowY;

  while (running) {
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Convert to seconds
    lastTime = currentTime;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }
    }

    // Get input
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_W]) {
      windowY -= speed * deltaTime;
    }
    if (keyState[SDL_SCANCODE_S]) {
      windowY += speed * deltaTime;
    }
    if (keyState[SDL_SCANCODE_A]) {
      windowX -= speed * deltaTime;
    }
    if (keyState[SDL_SCANCODE_D]) {
      windowX += speed * deltaTime;
    }

    // Move window
    SDL_SetWindowPosition(window, (int)windowX, (int)windowY);

    // Update player relative to window
    SDL_GetWindowPosition(window, &intWindowX, &intWindowY);
    Player.playerRect.x = Player.screenX - intWindowX;
    Player.playerRect.y = Player.screenY - intWindowY;

    // Draw
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, Player.playerSprite, NULL, &Player.playerRect);
    SDL_RenderPresent(renderer);

    // No need for SDL_Delay() now
  }
}
