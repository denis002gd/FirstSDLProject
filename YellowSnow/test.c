#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define FRAME_DELAY 16
#define SHAKE_DURATION 200
#define SHAKE_INTENSITY 10

typedef struct {
  int score;
  int clickAmount;
  int cpsLevel;
  Uint32 lastAutoClickTime;
  Uint32 clickInterval;

  bool isRockHovered;
  bool isRockClicked;
  Uint32 rockClickTime;
  bool isShaking;
} GameState;

typedef struct {
  SDL_Rect clickButton;
  SDL_Rect upgradeButton;
  SDL_Color backgroundColor;
  SDL_Color clickButtonColor;
  SDL_Color upgradeButtonColor;
  SDL_Color textColor;
  SDL_Texture *background;
  SDL_Texture *rock;
  SDL_Texture *clickUpgrade;
  SDL_Texture *CPSUpgrade;

  SDL_Rect clickBut;
  SDL_Rect cpsBut;
  SDL_Rect rockNormalRect;
  SDL_Rect rockHoverRect;
  SDL_Rect rockClickRect;
} UIElements;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  TTF_Font *smallFont;
  Mix_Chunk *clickSound;
} Resources;

bool initialize(Resources *res);
void cleanup(Resources *res, UIElements *ui);
bool isInsideRect(const SDL_Rect *rect, int x, int y);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                SDL_Color color, int x, int y);
void renderCenteredText(SDL_Renderer *renderer, TTF_Font *font,
                        const char *text, SDL_Color color, SDL_Rect container);
void handleEvents(bool *running, Resources *res, GameState *state,
                  UIElements *ui);
void update(GameState *state, UIElements *ui);
void render(Resources *res, GameState *state, UIElements *ui);
void initGameState(GameState *state);
void initUIElements(UIElements *ui);
void LoadImages(UIElements *ui, Resources *res);
bool loadData(GameState *gameState);
bool saveData(GameState *gameState);

int main() {
  Resources res = {0};
  GameState state;
  UIElements ui;
  bool running = true;

  if (!initialize(&res)) {
    return 1;
  }

  initGameState(&state);
  loadData(&state);
  initUIElements(&ui);
  LoadImages(&ui, &res);

  while (running) {
    handleEvents(&running, &res, &state, &ui);
    update(&state, &ui);
    render(&res, &state, &ui);
    SDL_Delay(FRAME_DELAY);
  }

  saveData(&state);
  cleanup(&res, &ui);
  return 0;
}

bool saveData(GameState *gameState) {
  FILE *fl = fopen("data.txt", "w");
  if (!fl) {
    return false;
  }
  fprintf(fl, "Score:%d\n", gameState->score);
  fprintf(fl, "TotalClicks:%d\n", gameState->clickAmount);
  fprintf(fl, "CPSLevel:%d\n", gameState->cpsLevel);
  fprintf(fl, "ClickInterval:%d", gameState->clickInterval);
  fclose(fl);
  return true;
}

bool loadData(GameState *gameState) {
  FILE *fl = fopen("data.txt", "r");
  if (!fl) {
    return false;
  }
  int score = 0;
  int clicks = 0;
  int cpsLevel = 1;
  Uint32 interval = 1000;
  char text[100];
  while (fgets(text, sizeof(text), fl)) {
    if (sscanf(text, "Score:%d", &score) == 1) {
      gameState->score = score;
    }
    if (sscanf(text, "TotalClicks:%d", &clicks) == 1) {
      gameState->clickAmount = clicks;
    }
    if (sscanf(text, "CPSLevel:%d", &cpsLevel) == 1) {
      gameState->cpsLevel = cpsLevel;
    }
    if (sscanf(text, "ClickInterval:%d", &interval) == 1) {
      gameState->clickInterval = interval;
    }
  }
  fclose(fl);
  return true;
}

bool initialize(Resources *res) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    printf("IMG_Init Error: %s\n", IMG_GetError());
    return false;
  }
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
           Mix_GetError());
    return false;
  }

  if (TTF_Init() < 0) {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }

  res->window =
      SDL_CreateWindow("Clicker Game", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (!res->window) {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  res->renderer = SDL_CreateRenderer(res->window, -1, SDL_RENDERER_ACCELERATED);
  if (!res->renderer) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  res->font =
      TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 28);
  if (!res->font) {
    printf("Failed to load font: %s\n", TTF_GetError());
    return false;
  }

  res->smallFont =
      TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
  if (!res->smallFont) {
    printf("Failed to load small font: %s\n", TTF_GetError());
    return false;
  }

  res->clickSound = Mix_LoadWAV("sounds/clicker.wav");
  if (!res->clickSound) {
    printf("Failed to load click sound effect! SDL_mixer Error: %s\n",
           Mix_GetError());
  }

  return true;
}

void cleanup(Resources *res, UIElements *ui) {
  if (res->font)
    TTF_CloseFont(res->font);
  if (res->smallFont)
    TTF_CloseFont(res->smallFont);
  if (res->clickSound)
    Mix_FreeChunk(res->clickSound);
  if (res->renderer)
    SDL_DestroyRenderer(res->renderer);
  if (res->window)
    SDL_DestroyWindow(res->window);
  if (ui->rock) {
    SDL_DestroyTexture(ui->rock);
  }
  if (ui->background) {
    SDL_DestroyTexture(ui->background);
  }
  if (ui->CPSUpgrade) {
    SDL_DestroyTexture(ui->CPSUpgrade);
  }
  if (ui->clickUpgrade) {
    SDL_DestroyTexture(ui->clickUpgrade);
  }

  Mix_CloseAudio();
  TTF_Quit();
  SDL_Quit();
  IMG_Quit();
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

void renderCenteredText(SDL_Renderer *renderer, TTF_Font *font,
                        const char *text, SDL_Color color, SDL_Rect container) {
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

  SDL_Rect dst = {container.x + (container.w - surface->w) / 2,
                  container.y + (container.h - surface->h) / 2, surface->w,
                  surface->h};
  dst.y -= 20;
  SDL_RenderCopy(renderer, texture, NULL, &dst);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void initGameState(GameState *state) {
  state->score = 0;
  state->clickAmount = 1;
  state->cpsLevel = 1;
  state->lastAutoClickTime = SDL_GetTicks();
  state->clickInterval = 1000;
  state->isRockHovered = false;
  state->isRockClicked = false;
  state->rockClickTime = 0;
  state->isShaking = false;
}

void initUIElements(UIElements *ui) {
  ui->clickButton = (SDL_Rect){400, 300, 200, 200};
  ui->upgradeButton = (SDL_Rect){400, 600, 200, 100};

  ui->rockNormalRect = (SDL_Rect){320, 250, 350, 300};
  ui->rockHoverRect = (SDL_Rect){310, 240, 370, 320};
  ui->rockClickRect = (SDL_Rect){330, 260, 330, 280};

  ui->backgroundColor = (SDL_Color){20, 20, 40, 255};
  ui->clickButtonColor = (SDL_Color){255, 0, 110, 255};
  ui->upgradeButtonColor = (SDL_Color){0, 255, 100, 255};
  ui->textColor = (SDL_Color){255, 240, 255, 255};

  ui->clickBut = (SDL_Rect){330, 500, 300, 200};
  ui->cpsBut = (SDL_Rect){330, 630, 300, 200};
}

void LoadImages(UIElements *ui, Resources *res) {
  ui->rock = IMG_LoadTexture(res->renderer, "art/rock.png");
  ui->background = IMG_LoadTexture(res->renderer, "art/background.png");
  ui->CPSUpgrade = IMG_LoadTexture(res->renderer, "art/buttonReady.png");
  ui->clickUpgrade = IMG_LoadTexture(res->renderer, "art/button.png");
}

void handleEvents(bool *running, Resources *res, GameState *state,
                  UIElements *ui) {
  SDL_Event e;
  int mx, my;

  SDL_GetMouseState(&mx, &my);
  state->isRockHovered = isInsideRect(&ui->clickButton, mx, my);

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      *running = false;
      break;

    case SDL_MOUSEBUTTONDOWN: {
      mx = e.button.x;
      my = e.button.y;

      if (isInsideRect(&ui->clickButton, mx, my)) {
        state->score += state->clickAmount;
        state->isRockClicked = true;
        state->rockClickTime = SDL_GetTicks();
        state->isShaking = true;

        if (res->clickSound) {
          Mix_PlayChannel(-1, res->clickSound, 0);
        }
      }

      int clickUpgradeCost = state->clickAmount * state->clickAmount;
      if (isInsideRect(&ui->clickBut, mx, my) &&
          state->score >= clickUpgradeCost) {
        state->score -= clickUpgradeCost;
        state->clickAmount++;
      }

      int cpsUpgradeCost = state->cpsLevel * state->cpsLevel * 100;
      if (isInsideRect(&ui->cpsBut, mx, my) && state->score >= cpsUpgradeCost) {
        state->score -= cpsUpgradeCost;
        state->cpsLevel++;
        state->clickInterval = (state->clickInterval > 50)
                                   ? state->clickInterval / 2
                                   : state->clickInterval;
      }
      break;
    }

    case SDL_MOUSEBUTTONUP: {
      if (state->isRockClicked) {
        state->isRockClicked = false;
      }
      break;
    }
    case SDL_KEYDOWN: {
      if (e.key.keysym.sym == SDLK_s) {
        if (saveData(state)) {
          printf("Game manually saved!\n");
        }
      }
      break;
    }
    }
  }
}

void update(GameState *state, UIElements *ui) {
  Uint32 currentTime = SDL_GetTicks();
  if (currentTime - state->lastAutoClickTime >= state->clickInterval) {
    state->score++;
    state->lastAutoClickTime = currentTime;
  }

  if (state->isShaking) {
    if (currentTime - state->rockClickTime > SHAKE_DURATION) {
      state->isShaking = false;
    }
  }
}

void render(Resources *res, GameState *state, UIElements *ui) {
  SDL_SetRenderDrawColor(res->renderer, ui->backgroundColor.r,
                         ui->backgroundColor.g, ui->backgroundColor.b,
                         ui->backgroundColor.a);
  SDL_RenderClear(res->renderer);

  SDL_Rect rockRect;

  if (state->isRockClicked) {
    rockRect = ui->rockClickRect;
  } else if (state->isRockHovered) {
    rockRect = ui->rockHoverRect;
  } else {
    rockRect = ui->rockNormalRect;
  }

  if (state->isShaking) {
    Uint32 elapsedTime = SDL_GetTicks() - state->rockClickTime;
    float shake_progress = (float)elapsedTime / SHAKE_DURATION;

    float intensity = SHAKE_INTENSITY * (1.0f - shake_progress);

    float shake_x = sin(shake_progress * 10) * intensity;
    float shake_y = cos(shake_progress * 12) * intensity;

    rockRect.x += (int)shake_x;
    rockRect.y += (int)shake_y;
  }

  SDL_RenderCopy(res->renderer, ui->background, NULL, NULL);
  SDL_RenderCopy(res->renderer, ui->rock, NULL, &rockRect);
  SDL_RenderCopy(res->renderer, ui->clickUpgrade, NULL, &ui->clickBut);
  SDL_RenderCopy(res->renderer, ui->CPSUpgrade, NULL, &ui->cpsBut);

  char scoreText[64];
  snprintf(scoreText, sizeof(scoreText), "Score: %d", state->score);
  renderText(res->renderer, res->smallFont, scoreText, ui->textColor, 20, 75);

  char clickText[64];
  snprintf(clickText, sizeof(clickText), "Click Value: %d", state->clickAmount);
  renderText(res->renderer, res->smallFont, clickText, ui->textColor, 20, 10);

  char cpsText[64];
  snprintf(cpsText, sizeof(cpsText), "CPS: %.1f",
           1000.0f / state->clickInterval);
  renderText(res->renderer, res->smallFont, cpsText, ui->textColor, 20, 45);

  char saveInstructions[64] = "press S to save";
  renderText(res->renderer, res->smallFont, saveInstructions, ui->textColor,
             750, 950);

  char clickUpgradeText[64];
  int clickUpgradeCost = state->clickAmount * state->clickAmount;
  snprintf(clickUpgradeText, sizeof(clickUpgradeText), "+1 Click: %d",
           clickUpgradeCost);
  renderCenteredText(res->renderer, res->smallFont, clickUpgradeText,
                     ui->textColor, ui->clickBut);

  char cpsUpgradeText[64];
  int cpsUpgradeCost = state->cpsLevel * state->cpsLevel * 100;
  snprintf(cpsUpgradeText, sizeof(cpsUpgradeText), "2x CPS: %d",
           cpsUpgradeCost);
  renderCenteredText(res->renderer, res->smallFont, cpsUpgradeText,
                     ui->textColor, ui->cpsBut);

  SDL_RenderPresent(res->renderer);
}
