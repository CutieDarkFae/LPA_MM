#include <stdio.h>
#include "fib_heap.h"

#define MAX_NEIGHBORS 4
typedef struct Cell {
    int x, y;
    double g;
    double rhs;
    double h;
    FibNode *heap_node;
    struct Cell *predecessors[MAX_NEIGHBORS];
    struct Cell *successors[MAX_NEIGHBORS];
    int num_succ;
    int num_pred;
} Cell;

int main() {
    printf("sizeof(Cell): %zu\n", sizeof(Cell));
    printf("sizeof(FibNode): %zu\n", sizeof(FibNode));
    printf("sizeof(double): %zu\n", sizeof(double));
    return 0;
}
