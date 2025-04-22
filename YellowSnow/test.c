#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Constants
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define FRAME_DELAY 16
#define AUTO_CLICK_INTERVAL 1000

// Game state
typedef struct {
  int score;
  int clickAmount;
  Uint32 lastAutoClickTime;
} GameState;

// UI Elements
typedef struct {
  SDL_Rect clickButton;
  SDL_Rect upgradeButton;
  SDL_Color backgroundColor;
  SDL_Color clickButtonColor;
  SDL_Color upgradeButtonColor;
  SDL_Color textColor;
} UIElements;

// Resources
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  Mix_Chunk *clickSound;
} Resources;

// Function prototypes
bool initialize(Resources *res);
void cleanup(Resources *res);
bool isInsideRect(const SDL_Rect *rect, int x, int y);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                SDL_Color color, int x, int y);
void handleEvents(bool *running, Resources *res, GameState *state,
                  UIElements *ui);
void update(GameState *state);
void render(Resources *res, GameState *state, UIElements *ui);
void initGameState(GameState *state);
void initUIElements(UIElements *ui);

int main() {
  Resources res = {0};
  GameState state;
  UIElements ui;
  bool running = true;

  if (!initialize(&res)) {
    return 1;
  }

  initGameState(&state);
  initUIElements(&ui);

  // Main game loop
  while (running) {
    handleEvents(&running, &res, &state, &ui);
    update(&state);
    render(&res, &state, &ui);
    SDL_Delay(FRAME_DELAY);
  }

  cleanup(&res);
  return 0;
}

bool initialize(Resources *res) {
  // Initialize SDL subsystems
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Initialize SDL_mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
           Mix_GetError());
    return false;
  }

  // Initialize SDL_ttf
  if (TTF_Init() < 0) {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }

  // Create window
  res->window =
      SDL_CreateWindow("Clicker Game", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (!res->window) {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Create renderer
  res->renderer = SDL_CreateRenderer(res->window, -1, SDL_RENDERER_ACCELERATED);
  if (!res->renderer) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Load font
  res->font =
      TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 32);
  if (!res->font) {
    printf("Failed to load font: %s\n", TTF_GetError());
    return false;
  }

  // Load sound effect
  res->clickSound = Mix_LoadWAV("sounds/click.wav");
  if (!res->clickSound) {
    printf("Failed to load click sound effect! SDL_mixer Error: %s\n",
           Mix_GetError());
    // Non-fatal error, continue without sound
  }

  return true;
}

void cleanup(Resources *res) {
  if (res->font)
    TTF_CloseFont(res->font);
  if (res->clickSound)
    Mix_FreeChunk(res->clickSound);
  if (res->renderer)
    SDL_DestroyRenderer(res->renderer);
  if (res->window)
    SDL_DestroyWindow(res->window);

  Mix_CloseAudio();
  TTF_Quit();
  SDL_Quit();
}

bool isInsideRect(const SDL_Rect *rect, int x, int y) {
  return x >= rect->x && x < rect->x + rect->w && y >= rect->y &&
         y < rect->y + rect->h;
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                SDL_Color color, int x, int y) {
  if (!text || !font)
    return;

  SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
  if (!surface) {
    printf("Unable to render text surface! SDL_ttf Error: %s\n",
           TTF_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    printf("Unable to create texture from rendered text! SDL Error: %s\n",
           SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect dst = {x, y, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, NULL, &dst);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void initGameState(GameState *state) {
  state->score = 0;
  state->clickAmount = 1;
  state->lastAutoClickTime = SDL_GetTicks();
}

void initUIElements(UIElements *ui) {
  // Main click button
  ui->clickButton = (SDL_Rect){400, 300, 200, 200};

  // Upgrade button
  ui->upgradeButton = (SDL_Rect){400, 600, 200, 100};

  // Colors
  ui->backgroundColor = (SDL_Color){30, 30, 30, 255};
  ui->clickButtonColor = (SDL_Color){200, 0, 0, 255};
  ui->upgradeButtonColor = (SDL_Color){0, 200, 0, 255};
  ui->textColor = (SDL_Color){255, 255, 255, 255};
}

void handleEvents(bool *running, Resources *res, GameState *state,
                  UIElements *ui) {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      *running = false;
      break;

    case SDL_MOUSEBUTTONDOWN: {
      int mx = e.button.x;
      int my = e.button.y;

      // Handle click button
      if (isInsideRect(&ui->clickButton, mx, my)) {
        state->score += state->clickAmount;
        if (res->clickSound) {
          Mix_PlayChannel(-1, res->clickSound, 0);
        }
      }

      // Handle upgrade button
      int upgradeCost = state->clickAmount * 10;
      if (isInsideRect(&ui->upgradeButton, mx, my) &&
          state->score >= upgradeCost) {
        state->score -= upgradeCost;
        state->clickAmount++;
      }
      break;
    }
    }
  }
}

void update(GameState *state) {
  // Auto-click logic (passive income)
  Uint32 currentTime = SDL_GetTicks();
  if (currentTime - state->lastAutoClickTime >= AUTO_CLICK_INTERVAL) {
    state->score++;
    state->lastAutoClickTime = currentTime;
  }
}

void render(Resources *res, GameState *state, UIElements *ui) {
  // Clear screen with background color
  SDL_SetRenderDrawColor(res->renderer, ui->backgroundColor.r,
                         ui->backgroundColor.g, ui->backgroundColor.b,
                         ui->backgroundColor.a);
  SDL_RenderClear(res->renderer);

  // Draw click button
  SDL_SetRenderDrawColor(res->renderer, ui->clickButtonColor.r,
                         ui->clickButtonColor.g, ui->clickButtonColor.b, 255);
  SDL_RenderFillRect(res->renderer, &ui->clickButton);

  // Draw upgrade button
  SDL_SetRenderDrawColor(res->renderer, ui->upgradeButtonColor.r,
                         ui->upgradeButtonColor.g, ui->upgradeButtonColor.b,
                         255);
  SDL_RenderFillRect(res->renderer, &ui->upgradeButton);

  // Render score text
  char scoreText[64];
  snprintf(scoreText, sizeof(scoreText), "Score: %d", state->score);
  renderText(res->renderer, res->font, scoreText, ui->textColor, 20, 20);

  // Render click amount text
  char clickText[64];
  snprintf(clickText, sizeof(clickText), "Click Value: %d", state->clickAmount);
  renderText(res->renderer, res->font, clickText, ui->textColor, 20, 70);

  // Render upgrade info
  char upgradeText[128];
  snprintf(upgradeText, sizeof(upgradeText), "Upgrade Cost: %d",
           state->clickAmount * 10);
  renderText(res->renderer, res->font, upgradeText, ui->textColor, 20,
             WINDOW_HEIGHT - 60);

  // Update screen
  SDL_RenderPresent(res->renderer);
}
