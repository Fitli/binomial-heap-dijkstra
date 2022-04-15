//
// Created by fitli on 14.04.22.
//

#include <stdio.h>
#include <stdlib.h>
#include "binomial_heap.h"

void destroy_tree(BinomialTree *root) {
    if(root == NULL) {
        return;
    }
    destroy_tree(root->child);
    destroy_tree(root->next_sibling);
    free(root);
}

void destroy_heap(Heap *heap) {
    destroy_tree(heap->first_root);
    free(heap);
}

void remove_tree(Heap *heap, BinomialTree *root) {
    if(root->prev_sibling) {
        root->prev_sibling->next_sibling = root->next_sibling;
    }
    if(root->next_sibling) {
        root->next_sibling->prev_sibling = root->prev_sibling;
    }
    if(heap->first_root == root) {
        heap->first_root = root->next_sibling;
    }
    heap->size -= 1 << root->order;
}

Heap *empty_heap() {
    Heap *heap = malloc(sizeof(Heap));
    heap->size = 0;
    heap->first_root = NULL;
    heap->min = NULL;
    return heap;
}

void find_min(Heap *heap) {
    BinomialTree *root = heap->first_root;
    heap->min = root;
    while (root != NULL) {
        if (root->key < heap->min->key) {
            heap->min = root;
        }
        root = root->next_sibling;
    }
}

int pop(Heap *heap) {
    if(heap->min == NULL) {
        return -1;
    }
    int result = heap->min->data;
    BinomialTree *removed_root = heap->min;

    Heap *children_heap = empty_heap();
    BinomialTree *new_root = removed_root->child;
    BinomialTree *next = NULL;
    while(new_root) {
        children_heap->size += 1 << new_root->order;
        if (children_heap->min == NULL || new_root->key < children_heap->min->key) {
            children_heap->min = new_root;
        }
        children_heap->first_root = new_root;
        new_root->prev_sibling = new_root->next_sibling;
        new_root->next_sibling = next;
        new_root->parent = NULL;
        next = new_root;
        new_root = new_root->prev_sibling;
    }

    remove_tree(heap, removed_root);

    merge(heap, children_heap);
    free(children_heap);
    free(heap->min);

    find_min(heap);

    return result;
}

BinomialTree *insert(Heap *heap, int data, double key) {
    BinomialTree *small_tree = malloc(sizeof(BinomialTree));
    small_tree->data = data;
    small_tree->key = key;
    small_tree->order = 0;
    small_tree->prev_sibling = NULL;
    small_tree->next_sibling = NULL;
    small_tree->child = NULL;
    small_tree->parent = NULL;


    Heap *small_heap = empty_heap();
    small_heap->first_root = small_tree;
    small_heap->min = small_tree;
    small_heap->size = 1;

    merge(heap, small_heap);
    free(small_heap);
    return small_tree;
}

BinomialTree *merge_trees(BinomialTree *a, BinomialTree *b) {
    if(a->key < b->key) {
        if (a->child)
            a->child->prev_sibling = b;
        b->next_sibling = a->child;
        b->prev_sibling = NULL;
        a->child = b;
        b->parent = a;
        a->order++;
        return a;
    }
    return merge_trees(b, a);
}

void merge(Heap *heap, Heap* added) {
    Heap output;
    output.first_root = NULL;
    output.size = heap->size + added->size;
    if(heap->min == NULL || (added->min != NULL && heap->min->key > added->min->key)) {
        output.min = added->min;
    } else {
        output.min = heap->min;
    }

    BinomialTree *last_new_root = NULL;
    BinomialTree *tr_h = heap->first_root;
    BinomialTree *tr_a = added->first_root;
    BinomialTree *to_add = NULL;

    while (tr_h || tr_a) {
        // One tree is empty - add the roots of the other to the end of the output root list
        if (tr_a == NULL) {
            to_add = tr_h;
        }
        if (tr_h == NULL) {
            to_add = tr_a;
        }
        if(to_add) {
            if (last_new_root == NULL) {
                output.first_root = to_add;
                to_add->prev_sibling = NULL;
            } else {
                last_new_root->next_sibling = to_add;
                to_add->prev_sibling = last_new_root;
            }
            break;
        }

        // add one new root
        if (tr_a == NULL || (tr_h != NULL && tr_h->order < tr_a->order)) {
            to_add = tr_h;
            tr_h = tr_h->next_sibling;
        } else if (tr_h == NULL || (tr_a != NULL && tr_h->order > tr_a->order)) {
            to_add = tr_a;
            tr_a = tr_a->next_sibling;
        } else {
            to_add = merge_trees(tr_h, tr_a);
            tr_h = tr_h->next_sibling;
            tr_a = tr_a->next_sibling;
        }

        if(last_new_root == NULL) {
            output.first_root = to_add;
            to_add->prev_sibling = NULL;
        } else if (to_add->order == last_new_root->order) {
            to_add = merge_trees(to_add, last_new_root);
            if(last_new_root->prev_sibling == NULL) {
                output.first_root = to_add;
                to_add->prev_sibling = NULL;
            } else {
                last_new_root->prev_sibling->next_sibling = to_add;
                to_add->prev_sibling = last_new_root->prev_sibling;
            }
        } else {
            last_new_root->next_sibling = to_add;
            to_add->prev_sibling = last_new_root;
        }
        to_add->next_sibling = NULL;
        last_new_root = to_add;
    }

    *heap = output;
}

void swap_nodes(BinomialTree *a, BinomialTree *b) {
    BinomialTree helper = *a;
    a->parent = b->parent;
    a->child = b->child;
    a->next_sibling = b->next_sibling;
    a->prev_sibling = b->prev_sibling;
    a->order = b->order;

    b->parent = helper.parent;
    b->child = helper.child;
    b->next_sibling = helper.next_sibling;
    b->prev_sibling = helper.prev_sibling;
    b->order = helper.order;
}

void lower_key(BinomialTree *node, double new_key) {
    node->key = new_key;
    BinomialTree *swap_with = node;
    while (swap_with->parent != NULL && swap_with->parent->key < new_key) {
        swap_with = swap_with->parent;
    }
    if (node != swap_with) {
        swap_nodes(node, swap_with);
    }
}
