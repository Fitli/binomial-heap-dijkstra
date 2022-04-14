//
// Created by fitli on 14.04.22.
//

#ifndef BINOMIAL_HEAP_DIJKSTRA_BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_DIJKSTRA_BINOMIAL_HEAP_H

typedef struct BinomialHeap {
    int size;
} Heap;

Heap *empty_heap();
int pop(Heap *heap);
void insert(Heap *heap, int data, double key);

#endif //BINOMIAL_HEAP_DIJKSTRA_BINOMIAL_HEAP_H
