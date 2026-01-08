#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "fib_heap.h"

// Helper function prototypes
static void consolidate(FibHeap *H);
static void fib_heap_link(FibHeap *H, FibNode *y, FibNode *x);
static void cut(FibHeap *H, FibNode *x, FibNode *y);
static void cascading_cut(FibHeap *H, FibNode *y);

// Compare keys: returns -1 if A < B, 1 if A > B, 0 if equal
int compare_keys(double a1, double a2, double b1, double b2) {
    if (a1 < b1) return -1;
    if (a1 > b1) return 1;
    if (a2 < b2) return -1;
    if (a2 > b2) return 1;
    return 0;
}

FibHeap* fib_heap_create() {
    FibHeap *H = (FibHeap*)malloc(sizeof(FibHeap));
    H->min = NULL;
    H->n = 0;
    return H;
}

FibNode* fib_heap_insert(FibHeap *H, double k1, double k2, void *payload) {
    FibNode *x = (FibNode*)malloc(sizeof(FibNode));
    x->k1 = k1;
    x->k2 = k2;
    x->payload = payload;
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
        
        if (compare_keys(x->k1, x->k2, H->min->k1, H->min->k2) < 0) {
            H->min = x;
        }
    }
    H->n++;
    return x;
}

int fib_heap_is_empty(FibHeap *H) {
    return H->min == NULL;
}

void* fib_heap_extract_min(FibHeap *H) {
    FibNode *z = H->min;
    if (z != NULL) {
        // Add each child of z to the root list
        if (z->child != NULL) {
            FibNode *child = z->child;
            do {
                FibNode *next = child->right;
                child->parent = NULL;
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
        void *payload = z->payload;
        free(z);
        return payload;
    }
    return NULL;
}

static void consolidate(FibHeap *H) {
    if (H->n == 0) return;
    
    // Max degree bound: log_phi(N). For simple upper bound 100 is enough for huge N.
    int D = 100; 
    FibNode **A = (FibNode**)calloc(D, sizeof(FibNode*));
    
    // Count roots to iterate safely
    int num_roots = 0;
    FibNode *x = H->min;
    if (x) {
        num_roots++;
        x = x->right;
        while (x != H->min) {
            num_roots++;
            x = x->right;
        }
    }

    FibNode **root_nodes = (FibNode**)malloc(num_roots * sizeof(FibNode*));
    x = H->min;
    for (int i = 0; i < num_roots; i++) {
        root_nodes[i] = x;
        x = x->right;
    }

    for (int i = 0; i < num_roots; i++) {
        FibNode *w = root_nodes[i];
        int d = w->degree;
        while (d < D && A[d] != NULL) {
            FibNode *y = A[d];
            // If w > y, swap
            if (compare_keys(w->k1, w->k2, y->k1, y->k2) > 0) {
                FibNode *temp = w;
                w = y;
                y = temp;
            }
            fib_heap_link(H, y, w);
            A[d] = NULL;
            d++;
        }
        if(d < D) A[d] = w;
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
                if (compare_keys(A[i]->k1, A[i]->k2, H->min->k1, H->min->k2) < 0) {
                    H->min = A[i];
                }
            }
        }
    }
    
    free(A);
    free(root_nodes);
}

static void fib_heap_link(FibHeap *H, FibNode *y, FibNode *x) {
    y->left->right = y->right;
    y->right->left = y->left;
    
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

void fib_heap_decrease_key(FibHeap *H, FibNode *x, double k1, double k2) {
    if (compare_keys(k1, k2, x->k1, x->k2) > 0) {
        // Only supports decrease key
        return;
    }
    x->k1 = k1;
    x->k2 = k2;
    FibNode *y = x->parent;
    if (y != NULL && compare_keys(x->k1, x->k2, y->k1, y->k2) < 0) {
        cut(H, x, y);
        cascading_cut(H, y);
    }
    if (compare_keys(x->k1, x->k2, H->min->k1, H->min->k2) < 0) {
        H->min = x;
    }
}

static void cut(FibHeap *H, FibNode *x, FibNode *y) {
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
    fib_heap_decrease_key(H, x, -DBL_MAX, -DBL_MAX);
    fib_heap_extract_min(H);
}

void fib_heap_free(FibHeap *H) {
    if (H) free(H);
    // Note: Does not free payloads or recursively free nodes here for simplicity
    // A full implementation would traverse and free.
}
