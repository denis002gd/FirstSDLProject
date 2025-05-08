#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  List *list = (List *)malloc(sizeof(List));
  InitList(list);
  for (int i = 0; i < 9; i++) {
    AddSimpleNode(list, i);
  }
  PrintSimpleList(list);
  Deallocate(list);
  return 0;
}
