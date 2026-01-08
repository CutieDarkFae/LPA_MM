#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "fib_heap.h"

// Helper function prototypes
static void consolidate(FibHeap *H);
static void fib_heap_link(FibHeap *H, FibNode *y, FibNode *x);
static void cut(FibHeap *H, FibNode *x, FibNode *y);
static void cascading_cut(FibHeap *H, FibNode *y);

FibHeap* fib_heap_create() {
    FibHeap *H = (FibHeap*)malloc(sizeof(FibHeap));
    H->min = NULL;
    H->n = 0;
    return H;
}

FibNode* fib_heap_insert(FibHeap *H, int key) {
    FibNode *x = (FibNode*)malloc(sizeof(FibNode));
    x->key = key;
    x->degree = 0;
    x->mark = 0;
    x->parent = NULL;
    x->child = NULL;
    x->left = x;
    x->right = x;

    if (H->min == NULL) {
        H->min = x;
    } else {
        // Insert x into root list
        x->left = H->min;
        x->right = H->min->right;
        H->min->right->left = x;
        H->min->right = x;
        
        if (x->key < H->min->key) {
            H->min = x;
        }
    }
    H->n++;
    return x;
}

int fib_heap_get_min(FibHeap *H) {
    if (H->min == NULL) return INT_MIN; // Or error code
    return H->min->key;
}

int fib_heap_extract_min(FibHeap *H) {
    FibNode *z = H->min;
    if (z != NULL) {
        // Add each child of z to the root list
        if (z->child != NULL) {
            FibNode *child = z->child;
            do {
                FibNode *next = child->right;
                
                // Reset parent pointer
                child->parent = NULL;
                
                // Add to root list
                child->left = H->min;
                child->right = H->min->right;
                H->min->right->left = child;
                H->min->right = child;
                
                child = next;
            } while (child != z->child);
        }
        
        // Remove z from root list
        z->left->right = z->right;
        z->right->left = z->left;
        
        if (z == z->right) {
            H->min = NULL;
        } else {
            H->min = z->right;
            consolidate(H);
        }
        H->n--;
        int minKey = z->key;
        free(z);
        return minKey;
    }
    return INT_MIN; // Empty heap
}

static void consolidate(FibHeap *H) {
    // Determine max degree (approx log base phi of n)
    int max_degree = (int)(log(H->n) / log(1.618)) + 2; 
    // Usually safe upper bound is slightly higher or fixed for simplicity
    // Let's use a safe static array size or dynamic if needed. 
    // For n up to huge numbers, 100 is plenty.
    int D = 100; 
    FibNode **A = (FibNode**)calloc(D, sizeof(FibNode*));
    
    // Iterate through root list nodes
    // Note: root list changes during consolidation, so we need to capture nodes carefully
    FibNode *start = H->min;
    if (!start) { free(A); return; }
    
    // Break the circular link for traversal to avoid infinite loops during linking
    // Or simpler: collect all root nodes into an array/list first
    int num_roots = 0;
    FibNode *count_node = H->min;
    if (count_node) {
        num_roots++;
        while (count_node->right != H->min) {
            num_roots++;
            count_node = count_node->right;
        }
    }
    
    FibNode **root_nodes = (FibNode**)malloc(num_roots * sizeof(FibNode*));
    FibNode *curr = H->min;
    for (int i = 0; i < num_roots; i++) {
        root_nodes[i] = curr;
        curr = curr->right;
    }

    for (int i = 0; i < num_roots; i++) {
        FibNode *x = root_nodes[i];
        int d = x->degree;
        while (A[d] != NULL) {
            FibNode *y = A[d];
            if (x->key > y->key) {
                FibNode *temp = x;
                x = y;
                y = temp;
            }
            fib_heap_link(H, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
    }
    
    H->min = NULL;
    for (int i = 0; i < D; i++) {
        if (A[i] != NULL) {
            if (H->min == NULL) {
                H->min = A[i];
                H->min->left = H->min;
                H->min->right = H->min;
            } else {
                A[i]->left = H->min;
                A[i]->right = H->min->right;
                H->min->right->left = A[i];
                H->min->right = A[i];
                if (A[i]->key < H->min->key) {
                    H->min = A[i];
                }
            }
        }
    }
    
    free(A);
    free(root_nodes);
}

static void fib_heap_link(FibHeap *H, FibNode *y, FibNode *x) {
    // Remove y from root list
    y->left->right = y->right;
    y->right->left = y->left;
    
    // Make y a child of x
    y->parent = x;
    if (x->child == NULL) {
        x->child = y;
        y->right = y;
        y->left = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = 0;
}

void fib_heap_decrease_key(FibHeap *H, FibNode *x, int k) {
    if (k > x->key) {
        // Error: new key is greater than current key
        return;
    }
    x->key = k;
    FibNode *y = x->parent;
    if (y != NULL && x->key < y->key) {
        cut(H, x, y);
        cascading_cut(H, y);
    }
    if (x->key < H->min->key) {
        H->min = x;
    }
}

static void cut(FibHeap *H, FibNode *x, FibNode *y) {
    // Remove x from child list of y
    if (x == x->right) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    y->degree--;
    
    // Add x to root list
    x->parent = NULL;
    x->left = H->min;
    x->right = H->min->right;
    H->min->right->left = x;
    H->min->right = x;
    
    x->mark = 0;
}

static void cascading_cut(FibHeap *H, FibNode *y) {
    FibNode *z = y->parent;
    if (z != NULL) {
        if (y->mark == 0) {
            y->mark = 1;
        } else {
            cut(H, y, z);
            cascading_cut(H, z);
        }
    }
}

void fib_heap_delete(FibHeap *H, FibNode *x) {
    fib_heap_decrease_key(H, x, INT_MIN);
    fib_heap_extract_min(H);
}
