
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
struct TreeNode {
  int val;
  struct TreeNode *left;
  struct TreeNode *right;
};

int *inorderTraversal(struct TreeNode *root, int *returnSize) {
  struct TreeNode *stack[100];
  int top = -1;
  int *answers = malloc(100 * sizeof(int));
  returnSize = 0;
  struct TreeNode *current = root;
  while (current != NULL || top != -1) {
    while (current != NULL) {
      stack[++top] = current;
      current = current->left;
    }
    current = stack[--top];
    answers[++(*returnSize)] = current->val;
    current = current->right;
  }
  return answers;
}

void intToBits(int num, int bits[], int size) {
  for (int i = 0; i < size; i++) {
    bits[size - 1 - i] = (num >> i) & 1;
  }
}
int bitsToInt(int bits[], int size) {
  int result = 0;
  for (int i = 0; i < size; i++) {
    result = (result << 1) | bits[i];
  }
  return result;
}
uint32_t reversedBitNum(uint32_t num) {
  int size = 32;
  int bits[size];
  for (int i = 0; i < size; i++) {
    bits[size - 1 - i] = (num >> i) & 1;
  }
  int bitsRev[size];
  int k = size - 1;
  for (int i = 0; i < size; i++)
    bitsRev[i] = bits[k--];

  uint32_t result = 0;
  for (int i = 0; i < size; i++) {
    result = (result << 1) | bitsRev[i];
  }
  return result;
}
bool isHappy(int n) {
  int tries = 10;
  int i;
  int sum = n;
  while (tries > 0) {
    int copy = sum;
    sum = 0;
    i = 0;
    while (copy > 0) {
      sum += (copy % 10) * (copy % 10);
      copy /= 10;
    }
    if (sum == 1) {
      return true;
    }

    tries--;
  }
  return false;
}
bool EstePatratPerfect(int n) {
  int low = 0;
  int high = n;
  while (low <= high) {
    int mid = low + (high - low) / 2;
    long long root = (long long)mid * mid;
    if (root == n) {
      return true;
    } else if (root < n) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return false;
}
int findPeriod(int arr[], int n) {
  if (n <= 1)
    return 0;

  for (int period = 1; period <= n / 2; period++) {
    bool isPeriodic = true;

    for (int i = 0; i < n; i++) {
      if (arr[i] != arr[i % period]) {
        isPeriodic = false;
        break;
      }
    }

    if (isPeriodic) {
      return (n / period);
    }
  }
  return 0;
}

int main() {
  int arr[15] = {1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3};
  int num = 253;
  int arrBits[8];
  intToBits(num, arrBits, 8);
  printf("the num %d to bits will be: \n", num);

  for (int i = 0; i < 8; i++) {
    printf("%d ", arrBits[i]);
  }
  printf("\nreversed: %d\n", reversedBitNum(00010001010010));
  printf("numarul %d este fericit: %d\n", 22, isHappy(19));
  if (EstePatratPerfect(10)) {
    printf("Este patrat perfect!\n");
  } else {
    printf("Nu este patrat perfect!\n");
  }
  printf("Este periodica: %d\n", findPeriod(arr, 15));
  return 0;
}
