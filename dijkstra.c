//
// Created by fitli on 14.04.22.
//

#include <float.h>
#include "dijkstra.h"
#include "binomial_heap.h"

double path_lenght(Graph *graph, int start, int end) {
    double lengths[graph->num_nodes];
    for(int i = 0; i < graph->num_nodes; i++) {
        lengths[i] = DBL_MAX;
    }
    Heap *heap = empty_heap();

    lengths[start] = 0;
    insert(heap, start, 0);

    while(heap->size > 0) {
        int node = pop(heap);
        for(int i = graph->starts[node]; i < graph->starts[node + 1]; i++) {
            int next_node = graph->edges[i].end;
            double weight = graph->edges[i].weight;
            if(lengths[node] + weight < lengths[next_node]) {
                lengths[next_node] = lengths[node] + weight;
                insert(heap, next_node, lengths[next_node]);
            }
        }
    }

    destroy_heap(heap);
    return lengths[end];
}
