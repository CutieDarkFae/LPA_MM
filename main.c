#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "fib_heap.h"

// Grid dimensions
#define ROWS 32
#define COLS 32
// How many neighbors each cell can have
#define MAX_NEIGHBORS 4
// Wall definitions
#define NORTH_WALL 1
#define EAST_WALL 2
#define SOUTH_WALL 4
#define WEST_WALL 8

// Cell structure for LPA*
typedef struct Cell {
    int x, y;
    double g;
    double rhs;
    double h; // heuristic
    FibNode *heap_node; // Pointer to node in heap, if present
    struct Cell *predecessors[MAX_NEIGHBORS]; // max 4 neighbors
    struct Cell *successors[MAX_NEIGHBORS];   // max 4 neighbors
    int num_succ;
    int num_pred;
} Cell;

typedef struct Graph {
    Cell *grid[ROWS][COLS];
    Cell *start;
    Cell *goal;
} Graph;

// Global priority queue
FibHeap *U;

// Function prototypes
void calculate_keys(Cell *u, double *k1, double *k2);
void update_vertex(Graph *G, Cell *u);
double heuristic(Cell *a, Cell *b);
void initialize(Graph *G);
void compute_shortest_path(Graph *G);
void print_path(Graph *G);
void print_maze(Graph *G);
void update_edge_cost(Graph *G, int x1, int y1, int x2, int y2, double new_cost);
int load_grid_from_csv(Graph *G, const char *filename);

// Global grid costs
int grid_costs[ROWS][COLS];

int walls[] = {EAST_WALL, WEST_WALL, NORTH_WALL, SOUTH_WALL};

int get_edge_cost(Cell *u, Cell *v) {
    // Return the cost stored in our grid_costs matrix
    return grid_costs[v->x][v->y]; 
}

int load_grid_from_csv(Graph *G, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open CSV file");
        return 0;
    }

    char line[1024];
    int row = 0;
    while (fgets(line, sizeof(line), file) && row < ROWS) {
        char *token = strtok(line, ",");
        int col = 0;
        while (token && col < COLS) {
            grid_costs[row][col] = atoi(token);
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    fclose(file);
    return 1;
}

double heuristic(Cell *a, Cell *b) {
    // Manhattan distance
    return fabs((double)a->x - b->x) + fabs((double)a->y - b->y);
}

void calculate_keys(Cell *u, double *k1, double *k2) {
    double min_g_rhs = (u->g < u->rhs) ? u->g : u->rhs;
    *k1 = min_g_rhs + u->h;
    *k2 = min_g_rhs;
}

void update_vertex(Graph *G, Cell *u) {
    if (u != G->start) {
        double min_val = DBL_MAX;
        for (int i = 0; i < u->num_pred; i++) {
            Cell *pred = u->predecessors[i];
            double cost = get_edge_cost(pred, u); // Simplified
            double val = pred->g + cost;
            if (val < min_val) min_val = val;
        }
        u->rhs = min_val;
    }

    if (u->heap_node != NULL) {
        fib_heap_delete(U, u->heap_node);
        u->heap_node = NULL;
    }

    if (u->g != u->rhs) {
        double k1, k2;
        calculate_keys(u, &k1, &k2);
        u->heap_node = fib_heap_insert(U, k1, k2, u);
    }
}

void initialize(Graph *G) {
    U = fib_heap_create();
    
    // Init grid
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            Cell *c = (Cell*)malloc(sizeof(Cell));
            c->x = i;
            c->y = j;
            c->g = DBL_MAX;
            c->rhs = DBL_MAX;
            c->heap_node = NULL;
            c->num_succ = 0;
            c->num_pred = 0;
            G->grid[i][j] = c;
            
            // Default cost if CSV not loaded or for initial state
            if (grid_costs[i][j] <= 0) grid_costs[i][j] = 1.0;
        }
    }

    // Set neighbors (Grid connectivity)
    int dx[] = {0, 0, 1, -1}; // these need to be expanded if MAX_NEIGHBORS chanages
    int dy[] = {1, -1, 0, 0};
    
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            Cell *c = G->grid[i][j];
            for(int k=0; k<4; k++) {
                int ni = i + dx[k];
                int nj = j + dy[k];
                if(ni >= 0 && ni < ROWS && nj >= 0 && nj < COLS) {
                    c->successors[c->num_succ++] = G->grid[ni][nj];
                    c->predecessors[c->num_pred++] = G->grid[ni][nj];
                }
            }
        }
    }

    G->start = G->grid[0][0];
    G->goal = G->grid[ROWS/2][COLS/2]; // Destination at middle

    // Init heuristic
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            G->grid[i][j]->h = heuristic(G->grid[i][j], G->goal);
        }
    }

    G->start->rhs = 0;
    
    double k1, k2;
    calculate_keys(G->start, &k1, &k2);
    G->start->heap_node = fib_heap_insert(U, k1, k2, G->start);
}

void compute_shortest_path(Graph *G) {
    while (!fib_heap_is_empty(U)) {
        FibNode *min_node = U->min;
        if (!min_node) break;
        
        double u_k1 = min_node->k1;
        double u_k2 = min_node->k2;
        
        double goal_k1, goal_k2;
        calculate_keys(G->goal, &goal_k1, &goal_k2);

        // compare_keys(A, B) returns -1 if A < B
        // Condition: TopKey < Key(goal) OR rhs(goal) != g(goal)
        if (compare_keys(u_k1, u_k2, goal_k1, goal_k2) >= 0 && G->goal->rhs == G->goal->g) {
            break;
        }

        Cell *u = (Cell*)fib_heap_extract_min(U);
        u->heap_node = NULL;

        if (u->g > u->rhs) {
            u->g = u->rhs;
            for (int i = 0; i < u->num_succ; i++) {
                update_vertex(G, u->successors[i]);
            }
        } else {
            u->g = DBL_MAX;
            update_vertex(G, u);
            for (int i = 0; i < u->num_succ; i++) {
                update_vertex(G, u->successors[i]);
            }
        }
    }
}

void print_path(Graph *G) {
    if (G->goal->g == DBL_MAX) {
        printf("No path found!\n");
        return;
    }
    printf("Path cost: %.1f\n", G->goal->g);
    
    // Simple backtrack for visualization (not part of LPA* core)
    // Note: This backtracking works for static snapshot, 
    // real LPA* would assume you just follow min g gradients
    printf("Path (backwards from goal): ");
    Cell *curr = G->goal;
    int steps = 0;
    while(curr != G->start && steps < ROWS*COLS) {
        printf("(%d,%d) <- ", curr->x, curr->y);
        double min_g = DBL_MAX;
        Cell *next = NULL;
        for(int i=0; i<curr->num_pred; i++) {
            Cell *pred = curr->predecessors[i];
            if(pred->g < min_g) {
                min_g = pred->g;
                next = pred;
            }
        }
        if(!next) break;
        curr = next;
        steps++;
    }
    printf("(%d,%d)\n", G->start->y, G->start->x);
}

void print_maze(Graph *G) {
    printf("\n--- Maze Visualization (Weights) ---\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid_costs[i][j] >= 100) {
                printf("[ # ] "); // Obstacle
            } else {
                printf("[%2.0f] ", grid_costs[i][j]);
            }
        }
        printf("\n");
    }

    printf("\n--- Cell Neighbors Example (Cell 0,0) ---\n");
    Cell *c = G->grid[0][0];
    printf("Cell (0,0) successors: ");
    for (int i = 0; i < c->num_succ; i++) {
        printf("(%d,%d) ", c->successors[i]->x, c->successors[i]->y);
    }
    printf("\n");
}

int main() {
    Graph G;
    
    // Try to load from grid.csv if it exists
    printf("Attempting to load grid from grid.csv...\n");
    if (load_grid_from_csv(&G, "maze_01.csv")) {
        printf("Grid loaded successfully.\n");
    } else {
        printf("Proceeding with default costs.\n");
    }

    printf("Initializing LPA* on %dx%d grid...\n", ROWS, COLS);
    
    initialize(&G);
    
    print_maze(&G);

    printf("\nComputing initial path...\n");
    compute_shortest_path(&G);
    print_path(&G);

    // Simulate change: Obstacle at (2,2)
    printf("\n--- Simulating Dynamic Change ---\n");
    printf("Adding obstacle at (2,2)...\n");
    grid_costs[2][2] = 100.0;
    
    // In LPA*, when an edge (u, v) changes, we call update_vertex(v)
    // We update the node (2,2) itself and its successors
    update_vertex(&G, G.grid[2][2]);
    for (int i = 0; i < G.grid[2][2]->num_succ; i++) {
        update_vertex(&G, G.grid[2][2]->successors[i]);
    }

    printf("Re-computing path...\n");
    compute_shortest_path(&G);
    print_path(&G);
    
    return 0;
}
