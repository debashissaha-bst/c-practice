#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 50


typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    AdjNode *head[MAX_VERTICES];
    int num_vertices;
} Graph;

static void graph_init(Graph *g, int num_vertices) {
    g->num_vertices = num_vertices;
    for (int i = 0; i < num_vertices; i++) g->head[i] = NULL;
}

static AdjNode *make_node(int v) {
    AdjNode *n = malloc(sizeof(AdjNode));
    n->vertex = v;
    n->next = NULL;
    return n;
}

static void graph_add_edge(Graph *g, int src, int dest) {
    AdjNode *n1 = make_node(dest);
    n1->next = g->head[src];
    g->head[src] = n1;

    AdjNode *n2 = make_node(src); 
    n2->next = g->head[dest];
    g->head[dest] = n2;
}

static void graph_free(Graph *g) {
    for (int i = 0; i < g->num_vertices; i++) {
        AdjNode *cur = g->head[i];
        while (cur) {
            AdjNode *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
}



typedef struct {
    int data[MAX_VERTICES];
    int front, rear, count;
} Queue;

static void queue_init(Queue *q) { q->front = 0; q->rear = -1; q->count = 0; }
static int  queue_empty(const Queue *q) { return q->count == 0; }

static void queue_push(Queue *q, int value) {
    q->rear = (q->rear + 1) % MAX_VERTICES;
    q->data[q->rear] = value;
    q->count++;
}

static int queue_pop(Queue *q) {
    int value = q->data[q->front];
    q->front = (q->front + 1) % MAX_VERTICES;
    q->count--;
    return value;
}



static void dfs_util(const Graph *g, int vertex, int *visited) {
    visited[vertex] = 1;
    printf("%d ", vertex);
    for (AdjNode *cur = g->head[vertex]; cur != NULL; cur = cur->next) {
        if (!visited[cur->vertex]) {
            dfs_util(g, cur->vertex, visited);
        }
    }
}

static void dfs(const Graph *g, int start) {
    int visited[MAX_VERTICES] = {0};
    printf("DFS from %d: ", start);
    dfs_util(g, start, visited);
    printf("\n");
}



static void bfs(const Graph *g, int start) {
    int visited[MAX_VERTICES] = {0};
    Queue q;
    queue_init(&q);

    visited[start] = 1;
    queue_push(&q, start);

    printf("BFS from %d: ", start);
    while (!queue_empty(&q)) {
        int vertex = queue_pop(&q);
        printf("%d ", vertex);
        for (AdjNode *cur = g->head[vertex]; cur != NULL; cur = cur->next) {
            if (!visited[cur->vertex]) {
                visited[cur->vertex] = 1;
                queue_push(&q, cur->vertex);
            }
        }
    }
    printf("\n");
}


static int is_connected(const Graph *g) {
    if (g->num_vertices == 0) return 1;
    int visited[MAX_VERTICES] = {0};
    Queue q;
    queue_init(&q);
    visited[0] = 1;
    queue_push(&q, 0);
    int reached = 1;

    while (!queue_empty(&q)) {
        int vertex = queue_pop(&q);
        for (AdjNode *cur = g->head[vertex]; cur != NULL; cur = cur->next) {
            if (!visited[cur->vertex]) {
                visited[cur->vertex] = 1;
                reached++;
                queue_push(&q, cur->vertex);
            }
        }
    }
    return reached == g->num_vertices;
}

int main(void) {
    
    Graph g;
    graph_init(&g, 6);
    graph_add_edge(&g, 0, 1);
    graph_add_edge(&g, 1, 2);
    graph_add_edge(&g, 0, 3);
    graph_add_edge(&g, 2, 5);
    graph_add_edge(&g, 3, 4);
    graph_add_edge(&g, 4, 5);

    dfs(&g, 0);
    bfs(&g, 0);

    printf("Graph connected? %s\n", is_connected(&g) ? "yes" : "no");

    graph_free(&g);
    return 0;
}
