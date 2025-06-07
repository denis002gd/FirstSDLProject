#ifndef LIST_H
#define LIST_H
#include <stdbool.h>
struct Node {
  int value;
  struct Node *next;
} typedef node_s;

typedef struct {
  node_s *Head;
} List;

typedef struct {
  float x;
  float y;
} Vector2;
//====VectorFunctions====

Vector2 V2Lerp(Vector2 destination, Vector2 target, int speed);
bool CompareVectors(Vector2 vec1, Vector2 vec2);
float V2Distance(Vector2 vect1, Vector2 vect2);
Vector2 V2Scale(Vector2 vect, float amount);
Vector2 Normalize(Vector2 vect);
Vector2 V2Sub(Vector2 a, Vector2 b);
Vector2 V2Add(Vector2 a, Vector2 b);

//=======================
void InitList(List *list);
node_s *CreateSimpleNode(int value);
void AddSimpleNode(List *list, int value);
void PrintSimpleList(List *list);
void Deallocate(List *list);
int IsInsideBox(int x, int y, int w, int h, int x1, int y1);
#endif // !LIST_H
