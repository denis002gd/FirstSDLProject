
#include "list.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void InitList(List *list) {
  if (list == NULL) {
    printf("List pointer is null.\n");
    return;
  }
  list->Head = NULL;
  printf("Initialisation success\n");
}
node_s *CreateSimpleNode(int value) {
  node_s *newNode = (node_s *)malloc(sizeof(node_s));
  if (newNode == NULL) {
    printf("Node allocation failed\n");
    exit(1);
  }
  newNode->value = value;
  newNode->next = NULL;
  return newNode;
}
void AddSimpleNode(List *list, int value) {
  node_s *newNode = CreateSimpleNode(value);
  newNode->next = list->Head;
  list->Head = newNode;
}
void PrintSimpleList(List *list) {
  node_s *temp = list->Head;
  while (temp) {
    printf("%d -> ", temp->value);
    temp = temp->next;
  }
  printf("NULL\n");
}

void Deallocate(List *list) {
  node_s *temp = list->Head;
  while (temp) {
    node_s *curent = temp;
    temp = temp->next;
    free(curent);
  }
  list->Head = NULL;
  printf("List was freed\n");
}

int IsInsideBox(int x, int y, int w, int h, int x1, int y1) {
  return (x1 >= x && x1 <= x + w && y1 >= y && y1 <= y + h);
}

int RandomInt(int min, int max) { return (rand() % max) + min; }
Vector2 V2Lerp(Vector2 destination, Vector2 target, int speed) {
  Vector2 direction = V2Sub(target, destination);
  float distance = V2Distance(target, destination);
  if (distance <= speed || distance == 0) {
    return target;
  }
  Vector2 norm = Normalize(direction);
  Vector2 step = V2Scale(norm, speed);
  Vector2 newPos = V2Add(destination, step);
  return newPos;
}

bool CompareVectors(Vector2 vec1, Vector2 vec2) {
  return (vec1.x == vec2.x && vec1.y == vec2.y);
}

float V2Distance(Vector2 vect1, Vector2 vect2) {
  float aPow = (vect1.x - vect2.x) * (vect1.x - vect2.x);
  float bPow = (vect1.y - vect2.y) * (vect1.y - vect2.y);
  float dis = sqrt(aPow + bPow);

  return dis;
}

Vector2 V2Scale(Vector2 vect, float amount) {
  Vector2 scaledVector = {vect.x * amount, vect.y * amount};
  return scaledVector;
}
Vector2 Normalize(Vector2 vect) {
  float aPow = vect.x * vect.x;
  float bPow = vect.y * vect.y;
  float mag = sqrt(aPow + bPow);

  float xNorm = vect.x / mag;
  float yNorm = vect.y / mag;
  Vector2 normalized = {xNorm, yNorm};
  return normalized;
}

Vector2 V2Sub(Vector2 a, Vector2 b) {
  Vector2 subtracted = {a.x - b.x, a.y - b.y};
  return subtracted;
}
Vector2 V2Add(Vector2 a, Vector2 b) {
  Vector2 added = {a.x + b.x, a.y + b.y};
  return added;
}
