//
// Created by fitli on 14.04.22.
//

#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include "dijkstra.h"
#include "binomial_heap.h"

#define LOG_OPERATIONS 0
#define DOT 0

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
    char filename[100];

    int iter = 0;
    while(heap->first_root) {
        if (LOG_OPERATIONS)
            printf("%d:",iter);
        int node = pop(heap);
        if (DOT) {
            sprintf(filename, "dot_inputs/dot_%d.dot", 1000 + iter);
            dot_printer(filename, heap);
        }

        if (DOT||LOG_OPERATIONS)
            iter++;

        in_heap[node] = NULL;
        for(int i = graph->starts[node]; i < graph->starts[node + 1]; i++) {
            int next_node = graph->edges[i].end;
            double weight = graph->edges[i].weight;
            if(lengths[node] + weight < lengths[next_node]) {
                lengths[next_node] = lengths[node] + weight;
                if (LOG_OPERATIONS)
                    printf("%d:",iter);
                if(in_heap[next_node]) {
                    lower_key(in_heap[next_node], lengths[next_node], heap);
                } else {
                    in_heap[next_node] = insert(heap, next_node, lengths[next_node]);
                }
                if (DOT) {
                    sprintf(filename, "dot_inputs/dot_%d.dot", 1000 + iter);
                    dot_printer(filename, heap);
                }

                if (DOT||LOG_OPERATIONS)
                    iter++;
            }
        }
    }

    printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", graph->num_nodes, graph->num_edges, graph->c,
           heap->stats.pops, heap->stats.inserts, heap->stats.decreases,
           heap->stats.tree_merges, heap->stats.root_removals, heap->stats.root_inserts, heap->stats.parent_child_swaps, heap->stats.find_min_iters);

    destroy_heap(heap);
    return lengths[end];
}
