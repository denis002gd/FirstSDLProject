#include "list.h"
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
