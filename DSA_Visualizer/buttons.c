#include "buttons.h"
#include <strings.h>

void TraverseLinkedList(Node_v *root, double *currentTime, double delayTime,
                        double deltaTime, int totalNodes) {
  static int currentIndex = 0;

  *currentTime += deltaTime;

  if (*currentTime < delayTime) {
    return;
  } else {
    *currentTime = 0;
  }

  Node_v *temp = root;
  while (temp != NULL) {
    temp->isSelected = false;
    temp = temp->next;
  }

  if (currentIndex < totalNodes) {
    temp = root;
    int nodeIndex = 0;

    while (temp != NULL && nodeIndex < currentIndex) {
      temp = temp->next;
      nodeIndex++;
    }

    if (temp != NULL) {
      temp->isSelected = true;
    }

    currentIndex++;
  } else {
    currentIndex = 0;
  }
}
