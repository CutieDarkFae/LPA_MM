#include <stdio.h>
#include "fib_heap.h"

int main() {
    printf("Fibonacci Heap Test\n");
    
    FibHeap *H = fib_heap_create();
    
    printf("Inserting 10, 3, 15, 6\n");
    FibNode *n10 = fib_heap_insert(H, 10);
    FibNode *n3 = fib_heap_insert(H, 3);
    FibNode *n15 = fib_heap_insert(H, 15);
    FibNode *n6 = fib_heap_insert(H, 6);
    
    printf("Current Min: %d\n", fib_heap_get_min(H));
    
    printf("Extract Min: %d\n", fib_heap_extract_min(H));
    printf("New Min: %d\n", fib_heap_get_min(H));
    
    printf("Decrease key of 15 to 2\n");
    fib_heap_decrease_key(H, n15, 2);
    printf("New Min: %d\n", fib_heap_get_min(H));
    
    printf("Extract Min: %d\n", fib_heap_extract_min(H));
    printf("Extract Min: %d\n", fib_heap_extract_min(H));
    printf("Extract Min: %d\n", fib_heap_extract_min(H));
    
    printf("Is empty? Min key: %d\n", fib_heap_get_min(H));

    return 0;
}
