#ifndef LIST_H
#define LIST_H

struct Node {
  int value;
  struct Node *next;
} typedef node_s;

typedef struct List {
  node_s *Head;
} List;

void InitList(List *list);
node_s *CreateSimpleNode(int value);
void AddSimpleNode(List *list, int value);
void PrintSimpleList(List *list);
void Deallocate(List *list);
#endif // !LIST_H
