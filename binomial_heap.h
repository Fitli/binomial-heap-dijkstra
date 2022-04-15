//
// Created by fitli on 14.04.22.
//

#ifndef BINOMIAL_HEAP_DIJKSTRA_BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_DIJKSTRA_BINOMIAL_HEAP_H

typedef struct BinomialTree BinomialTree;

struct BinomialTree {
    int data;
    double key;

    int order;
    BinomialTree *child;
    BinomialTree *parent;
    BinomialTree *prev_sibling;
    BinomialTree *next_sibling;
};

typedef struct BinomialHeap {
    int size;
    BinomialTree *first_root;
    BinomialTree *min;
} Heap;

void destroy_heap(Heap *heap);
Heap *empty_heap();
int pop(Heap *heap);
BinomialTree *insert(Heap *heap, int data, double key);
void merge(Heap *heap, Heap* added);
void lower_key(BinomialTree *node, double new_key);

#endif //BINOMIAL_HEAP_DIJKSTRA_BINOMIAL_HEAP_H
