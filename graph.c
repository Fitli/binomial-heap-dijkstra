//
// Created by fitli on 14.04.22.
//

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "graph.h"

// creates random orieanted graph
Graph *random_graph(int size, int c ) {
    srand(time(NULL));

    Graph *graph = malloc(sizeof(Graph));
    graph->num_edges = 0;
    graph->num_nodes = size;
    int cap_edges = 128;
    graph->edges = malloc(sizeof(Edge) * cap_edges);
    graph->starts = malloc(sizeof(int) * (size + 1));

    double p = c * log(size) / size;
    for (int i = 0; i<size; ++ i ) {
        graph->starts[i] = graph->num_edges;
        for (int j = 0; j<size; ++ j ) {
            double r = (float) rand()/(float) RAND_MAX;
            if ( r < p ) {
                if (cap_edges <= graph->num_edges) {
                    cap_edges *= 2;
                    graph->edges = realloc(graph->edges, sizeof(Edge) * cap_edges);
                }
                graph->edges[graph->num_edges].start = i;
                graph->edges[graph->num_edges].end = j;
                graph->edges[graph->num_edges].weight = (float) rand()/(float) RAND_MAX;
                graph->num_edges++;
            }
        }
    }
    graph->starts[graph->num_nodes] = graph->num_edges;
    return graph;
}

void print_dot(Graph *graph) {
    for (int i = 0; i < graph->num_edges; i++) {
        printf("%d -> %d\n", graph->edges[i].start, graph->edges[i].end);
    }
}