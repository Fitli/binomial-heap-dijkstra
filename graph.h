//
// Created by fitli on 14.04.22.
//

#ifndef BINOMIAL_HEAP_DIJKSTRA_GRAPH_H
#define BINOMIAL_HEAP_DIJKSTRA_GRAPH_H

typedef struct Edge {
    int start;
    int end;
    double weight;
} Edge;

typedef struct Graph {
    Edge *edges;
    int num_edges;
    int num_nodes;
    int *starts;
} Graph;

Graph *random_graph(int size, int c );
void print_dot(Graph *graph);
void destroy_graph(Graph *graph);

#endif //BINOMIAL_HEAP_DIJKSTRA_GRAPH_H
