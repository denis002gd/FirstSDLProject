#ifndef RENDER_H
#define RENDER_H
#include "list.h"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#define HEIGHT 1000
#define WIDTH 1600
#define RECT_PADDING 5
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 90
#define BUTTON_SPACING 10
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
#define REFRESHRATE 16

#define MAXNODES 20
#define NODE_WIDTH 150
#define NODE_HEIGHT 90
// h
#include <SDL2/SDL.h>

typedef struct Resources {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Color *bgColor;
  Mix_Music *clickSFX;
  Mix_Music *errorSFX;
  Mix_Music *spawnSFX;
  int nodesNumber;
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
struct InputField {
  char *input;
  bool active;
  SDL_Rect position;
  SDL_Rect bgPos;
  TTF_Font *font;
  const int fontSize;
  SDL_Color textColor;
  SDL_Texture *textTexture;
  SDL_Texture *background;
  SDL_Texture *deselectedBG;
};
typedef struct NodeVisual {
  Vector2 position;
  bool isMoving;
  struct NodeVisual *next;
  SDL_Texture *texture;
  bool isSelected;
  SDL_Texture *SelectedTexture;
  SDL_Rect rect;
  List *nodeList;
  int nodeIndex;

  char *text;
  TTF_Font *font;
  SDL_Texture *textTexture;
  SDL_Rect textRect;
} Node_v;
//=========Essentials====================
int InitProgram(Res *resources);
void CleanupProgram(Res *resources);
//=========Audio=========================
void PlayAudio(Res *resources, int index);
//=========Buttons=======================
int InitButton(Res *resources, struct Button *button, char *path,
               char *secondPath);
void DrawButton(Res *resources, struct Button *button);
void InitButtonText(Res *res, struct Button *button, const char *text,
                    int textSize, const char *font);
void CleanUpButton(struct Button *button);
//=========Panels========================
int InitPanel(Res *res, struct Panel *panel);

void RenderPanel(Res *res, struct Panel *panel);
void CleanupPanel(struct Panel *panel);
//========Input==========================
int InitInputField(Res *resurces, struct InputField *InputField, SDL_Rect rect,
                   const char *textFont, char *text, const int textSize);
void UpdateInput(Res *resurces, struct InputField *InputField, char *newInput);
//========Nodes==========================
int InitNode(Node_v *node, Res *resurces, char *text, SDL_Rect rect, int index);
void UpdateList(Res *resources, Node_v *node_v);
void FreeNodesInfo(Node_v **node);
Node_v *AddNodeToList(Res *resources, Node_v **Root, int value);
void CheckList(Node_v *node, int mouseX, int mouseY, bool isClick);
//=======================================
#endif // !DEBUG:
