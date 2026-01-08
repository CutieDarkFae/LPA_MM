#ifndef FIB_HEAP_H
#define FIB_HEAP_H

#include <limits.h>

// Node structure for Fibonacci Heap
typedef struct FibNode {
    int key;
    int degree;
    int mark; // 1 if node has lost a child since it became a child of another node
    struct FibNode *parent;
    struct FibNode *child;
    struct FibNode *left;
    struct FibNode *right;
} FibNode;

// Fibonacci Heap structure
typedef struct FibHeap {
    FibNode *min;
    int n; // Number of nodes
} FibHeap;

// Function prototypes
FibHeap* fib_heap_create();
FibNode* fib_heap_insert(FibHeap *H, int key);
int fib_heap_get_min(FibHeap *H);
int fib_heap_extract_min(FibHeap *H);
void fib_heap_decrease_key(FibHeap *H, FibNode *x, int k);
void fib_heap_delete(FibHeap *H, FibNode *x);
void fib_heap_free(FibHeap *H);

#endif
