//
// Created by fitli on 14.04.22.
//

#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include "dijkstra.h"
#include "binomial_heap.h"

double path_lenght(Graph *graph, int start, int end) {
    double lengths[graph->num_nodes];
    for(int i = 0; i < graph->num_nodes; i++) {
        lengths[i] = DBL_MAX;
    }

    Heap *heap = empty_heap();
    BinomialTree *in_heap[graph->num_nodes];
    for(int i = 0; i < graph->num_nodes; i++) {
        in_heap[i] = NULL;
    }

    lengths[start] = 0;
    in_heap[start] = insert(heap, start, 0);

    int iter = 0;
    while(heap->first_root) {
        int node = pop(heap);
        in_heap[node] = NULL;
        for(int i = graph->starts[node]; i < graph->starts[node + 1]; i++) {
            int next_node = graph->edges[i].end;
            double weight = graph->edges[i].weight;
            if(lengths[node] + weight < lengths[next_node]) {
                lengths[next_node] = lengths[node] + weight;
                if(in_heap[next_node]) {
                    lower_key(in_heap[next_node], lengths[next_node], heap);
                } else {
                    in_heap[next_node] = insert(heap, next_node, lengths[next_node]);
                }
            }
            char filename[100];
            sprintf(filename, "dot_inputs/dot_%d.dot", 1000+iter);
            dot_printer(filename, heap);
            iter++;
            if(iter > 100) {
                break;
            }
        }
        if(iter > 100) {
            break;
        }
    }

    printf("%d %d %d %d\n", heap->stats.root_inserts, heap->stats.root_removals, heap->stats.tree_merges, heap->stats.parent_child_swaps);

    destroy_heap(heap);
    return lengths[end];
}
