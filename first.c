#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GAME_NAME "My Clang Game"
#define LENGTH 1600
#define HEIGHT 900
#define STANDARTFPS 16
#define TEXT_SIZE 80
#define PLAYER_SIZE 30

struct Game {
  SDL_Renderer *renderer;
  SDL_Window *window;
  SDL_Texture *background;
  TTF_Font *textFont;
  SDL_Color text_color;
  SDL_Rect text_rect;
  SDL_Texture *text_texture;
  int text_vel_x;
  int text_vel_y;
  SDL_Texture *player_texture;
  SDL_Rect player_rect;
  int player_speed;
  const Uint8 *keyState;
  Mix_Chunk *sound;
  Mix_Chunk *susSound;
};

int Initializare(struct Game *game) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stdout, "SDL a esuat la initializare: %s", SDL_GetError());
    return true;
  }
  int init_mix = Mix_Init(MIX_INIT_MP3);
  if ((init_mix & MIX_INIT_MP3) != MIX_INIT_MP3) {
    fprintf(stdout, "SDL a esuat la initializarea sunetului: %s",
            Mix_GetError());
    return 0;
  }
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                    MIX_DEFAULT_CHANNELS, 1024)) {
    fprintf(stdout, "Esuare la deschiderea sunetului: %s", Mix_GetError());
    return 0;
  }
  game->sound = Mix_LoadWAV("sounds/pong.mp3");

  game->susSound = Mix_LoadWAV("sounds/amogsus.mp3");

  int init_img = IMG_Init(IMG_INIT_PNG);
  if ((init_img & IMG_INIT_PNG) != IMG_INIT_PNG) {
    fprintf(stdout, "SDL a esuat la initializarea imaginii: %s",
            IMG_GetError());
    return 0;
  }
  if (TTF_Init() == -1) {
    fprintf(stdout, "TTF a esuat la initializare: %s", TTF_GetError());
    return true;
  }
  game->window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, LENGTH, HEIGHT, 0);
  if (!game->window) {
    fprintf(stdout, "SDL a esuat la crearea ferestrei: %s", SDL_GetError());
    return true;
  }
  game->renderer = SDL_CreateRenderer(game->window, -1, 0);
  if (!game->renderer) {
    fprintf(stdout, "SDL a esuat la crearea renderului: %s", SDL_GetError());
  }
  return false;
}

bool LoadTexture(struct Game *game) {

  game->background = IMG_LoadTexture(game->renderer, "Images/background.png");
  if (!game->background) {
    fprintf(stdout, "A esuat crearea imaginii: %s", IMG_GetError());
    return true;
  }
  game->player_texture = IMG_LoadTexture(game->renderer, "Images/amogus.png");
  if (!game->player_texture) {
    fprintf(stdout, "A esuat crearea imaginii playerului: %s", IMG_GetError());
    return true;
  }

  if (SDL_QueryTexture(game->player_texture, NULL, NULL, &game->player_rect.w,
                       &game->player_rect.h)) {
    fprintf(stdout, "Image Query a esuat: %s", IMG_GetError());
    return true;
  }
  game->textFont =
      TTF_OpenFont("Fonts/Roboto/static/Roboto-Black.ttf", TEXT_SIZE);

  if (!game->textFont) {
    fprintf(stdout, "A esuat crearea textului: %s", TTF_GetError());
    return true;
  }
  SDL_Surface *surface =
      TTF_RenderText_Blended(game->textFont, "Hello world", game->text_color);
  if (!game->textFont) {
    fprintf(stdout, "A esuat crearea surface: %s", TTF_GetError());
    return true;
  }
  game->text_rect.w = surface->w;
  game->text_rect.h = surface->h;
  game->text_rect.x = (LENGTH - surface->w) / 2;
  game->text_rect.y = (HEIGHT - surface->h) / 2;
  game->text_texture = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);
  return false;
}

void MiscaText(struct Game *game) {
  game->text_rect.x += game->text_vel_x;
  game->text_rect.y += game->text_vel_y;

  if (game->text_rect.x + game->text_rect.w > LENGTH) {
    game->text_vel_x = -3;
    Mix_PlayChannel(-1, game->sound, 0);
  }
  if (game->text_rect.x < 0) {
    game->text_vel_x = 3;

    Mix_PlayChannel(-1, game->sound, 0);
  }
  if (game->text_rect.y + game->text_rect.h > HEIGHT) {
    game->text_vel_y = -3;
    Mix_PlayChannel(-1, game->sound, 0);
  }
  if (game->text_rect.y < 0) {
    game->text_vel_y = 3;
    Mix_PlayChannel(-1, game->sound, 0);
  }
  game->text_color.r = rand() % 255;
  game->text_color.g = rand() % 255;
  game->text_color.b = rand() % 255;
  SDL_Surface *surface =
      TTF_RenderText_Blended(game->textFont, "Hello world", game->text_color);

  game->text_texture = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);
}

void TerminareProgram(struct Game *game) {
  Mix_HaltChannel(-1);
  Mix_FreeChunk(game->sound);
  Mix_FreeChunk(game->susSound);
  Mix_CloseAudio();
  TTF_CloseFont(game->textFont);
  SDL_DestroyTexture(game->player_texture);
  SDL_DestroyTexture(game->background);
  SDL_DestroyTexture(game->text_texture);
  SDL_DestroyRenderer(game->renderer);
  SDL_DestroyWindow(game->window);
  Mix_Quit();
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
}

void PlayerMovement(struct Game *game) {
  if (game->keyState[SDL_SCANCODE_A]) {
    game->player_rect.x -= game->player_speed;
  }
  if (game->keyState[SDL_SCANCODE_D]) {
    game->player_rect.x += game->player_speed;
  }
  if (game->keyState[SDL_SCANCODE_W]) {
    game->player_rect.y -= game->player_speed;
  }
  if (game->keyState[SDL_SCANCODE_S]) {
    game->player_rect.y += game->player_speed;
  }
}

int main() {
  srand(time(NULL));
  int running = 1;

  int red, green, blue = 0;

  struct Game game = {
      .renderer = NULL,
      .window = NULL,
      .background = NULL,
      .textFont = NULL,
      .text_color = {255, 255, 255},
      .text_texture = NULL,
      .text_vel_x = 3,
      .text_vel_y = 3,
      .player_texture = NULL,
      .player_rect = {PLAYER_SIZE, PLAYER_SIZE, 0, 0},
      .player_speed = 5,
      .keyState = SDL_GetKeyboardState(NULL),
      .sound = NULL,
      .susSound = NULL,

  };

  if (Initializare(&game)) {
    TerminareProgram(&game);
  }
  if (LoadTexture(&game)) {
    TerminareProgram(&game);
  }
  SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        TerminareProgram(&game);
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
          TerminareProgram(&game);
          break;
        case SDL_SCANCODE_SPACE:
          SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
          Mix_PlayChannel(-1, game.susSound, 0);
          break;
        case SDL_SCANCODE_1:
          red += 5;
          SDL_SetRenderDrawColor(game.renderer, red, green, blue, 255);
          break;
        case SDL_SCANCODE_2:
          green += 5;
          SDL_SetRenderDrawColor(game.renderer, red, green, blue, 255);
          break;
        case SDL_SCANCODE_3:
          blue += 5;
          SDL_SetRenderDrawColor(game.renderer, red, green, blue, 255);
          break;
        default:
          break;
        }
      default:
        break;
      }
    }
    PlayerMovement(&game);
    SDL_RenderClear(game.renderer);
    MiscaText(&game);
    SDL_RenderCopy(game.renderer, game.background, NULL, NULL);
    SDL_RenderCopy(game.renderer, game.player_texture, NULL, &game.player_rect);
    SDL_RenderCopy(game.renderer, game.text_texture, NULL, &game.text_rect);
    SDL_RenderPresent(game.renderer);
    SDL_Delay(STANDARTFPS);
  }
}
