#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VERTICES 100

typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;
} Edge;

typedef struct {
    Edge *head[MAX_VERTICES];
    int num_vertices;
} Graph;

static void graph_init(Graph *g, int num_vertices) {
    g->num_vertices = num_vertices;
    for (int i = 0; i < num_vertices; i++) g->head[i] = NULL;
}

static void graph_add_edge(Graph *g, int src, int dest, int weight) {
    Edge *e1 = malloc(sizeof(Edge));
    e1->dest = dest; e1->weight = weight; e1->next = g->head[src];
    g->head[src] = e1;

    Edge *e2 = malloc(sizeof(Edge));
    e2->dest = src; e2->weight = weight; e2->next = g->head[dest];
    g->head[dest] = e2;
}

static void graph_free(Graph *g) {
    for (int i = 0; i < g->num_vertices; i++) {
        Edge *cur = g->head[i];
        while (cur) {
            Edge *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
}



typedef struct {
    int vertex;
    int dist;
} HeapNode;

typedef struct {
    HeapNode nodes[MAX_VERTICES];
    int pos[MAX_VERTICES]; 
    int size;
} MinHeap;

static void heap_init(MinHeap *h) {
    h->size = 0;
    for (int i = 0; i < MAX_VERTICES; i++) h->pos[i] = -1;
}

static void heap_swap(MinHeap *h, int i, int j) {
    HeapNode tmp = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = tmp;
    h->pos[h->nodes[i].vertex] = i;
    h->pos[h->nodes[j].vertex] = j;
}

static void heap_sift_down(MinHeap *h, int i) {
    int smallest = i;
    int l = 2 * i + 1, r = 2 * i + 2;
    if (l < h->size && h->nodes[l].dist < h->nodes[smallest].dist) smallest = l;
    if (r < h->size && h->nodes[r].dist < h->nodes[smallest].dist) smallest = r;
    if (smallest != i) {
        heap_swap(h, i, smallest);
        heap_sift_down(h, smallest);
    }
}

static void heap_sift_up(MinHeap *h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->nodes[parent].dist <= h->nodes[i].dist) break;
        heap_swap(h, i, parent);
        i = parent;
    }
}

static void heap_push(MinHeap *h, int vertex, int dist) {
    int i = h->size++;
    h->nodes[i].vertex = vertex;
    h->nodes[i].dist = dist;
    h->pos[vertex] = i;
    heap_sift_up(h, i);
}

static int heap_empty(const MinHeap *h) {
    return h->size == 0;
}

static HeapNode heap_pop_min(MinHeap *h) {
    HeapNode top = h->nodes[0];
    h->pos[top.vertex] = -1;
    h->size--;
    if (h->size > 0) {
        h->nodes[0] = h->nodes[h->size];
        h->pos[h->nodes[0].vertex] = 0;
        heap_sift_down(h, 0);
    }
    return top;
}


static void heap_decrease_key(MinHeap *h, int vertex, int new_dist) {
    int i = h->pos[vertex];
    if (i == -1) return;
    h->nodes[i].dist = new_dist;
    heap_sift_up(h, i);
}

static int heap_contains(const MinHeap *h, int vertex) {
    return h->pos[vertex] != -1;
}



static void dijkstra(Graph *g, int source, int *dist) {
    int visited[MAX_VERTICES] = {0};
    for (int i = 0; i < g->num_vertices; i++) dist[i] = INT_MAX;
    dist[source] = 0;

    MinHeap heap;
    heap_init(&heap);
    for (int i = 0; i < g->num_vertices; i++) {
        heap_push(&heap, i, dist[i]);
    }

    while (!heap_empty(&heap)) {
        HeapNode current = heap_pop_min(&heap);
        int u = current.vertex;
        if (dist[u] == INT_MAX) break; 
        if (visited[u]) continue;
        visited[u] = 1;

        for (Edge *e = g->head[u]; e != NULL; e = e->next) {
            int v = e->dest;
            if (!visited[v] && dist[u] != INT_MAX &&
                dist[u] + e->weight < dist[v]) {
                dist[v] = dist[u] + e->weight;
                if (heap_contains(&heap, v)) {
                    heap_decrease_key(&heap, v, dist[v]);
                }
            }
        }
    }
}

int main(void) {
    
    int num_vertices = 8;
    Graph g;
    graph_init(&g, num_vertices);

    graph_add_edge(&g, 0, 1, 8);
    graph_add_edge(&g, 0, 7, 11);
    graph_add_edge(&g, 1, 2, 4);
    graph_add_edge(&g, 1, 7, 1);
    graph_add_edge(&g, 2, 3, 9);
    graph_add_edge(&g, 2, 4, 2);
    graph_add_edge(&g, 2, 6, 7);
    graph_add_edge(&g, 3, 4, 9);
    graph_add_edge(&g, 3, 5, 14);
    graph_add_edge(&g, 4, 5, 10);
    graph_add_edge(&g, 5, 6, 2);
    graph_add_edge(&g, 6, 7, 1);

    int dist[MAX_VERTICES];
    int source = 0;
    dijkstra(&g, source, dist);

    printf("Shortest distances from vertex %d:\n", source);
    for (int i = 0; i < num_vertices; i++) {
        if (dist[i] == INT_MAX) {
            printf("  Vertex %d: unreachable\n", i);
        } else {
            printf("  Vertex %d: %d\n", i, dist[i]);
        }
    }

    graph_free(&g);
    return 0;
}
