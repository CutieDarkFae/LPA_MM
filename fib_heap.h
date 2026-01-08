#ifndef FIB_HEAP_H
#define FIB_HEAP_H

#include <limits.h>

// Node structure for Fibonacci Heap
typedef struct FibNode {
    double k1;          // Primary key
    double k2;          // Secondary key (tie-breaker)
    void *payload;      // Pointer to the graph node (cell)
    
    int degree;
    int mark;
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
FibNode* fib_heap_insert(FibHeap *H, double k1, double k2, void *payload);
void* fib_heap_extract_min(FibHeap *H); // Returns payload
int fib_heap_is_empty(FibHeap *H);
void fib_heap_decrease_key(FibHeap *H, FibNode *x, double k1, double k2);
void fib_heap_delete(FibHeap *H, FibNode *x);
void fib_heap_free(FibHeap *H);

// Helper to compare keys
int compare_keys(double a1, double a2, double b1, double b2);

#endif
