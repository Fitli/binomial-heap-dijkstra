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
    heap->stats.root_removals++;
}

Heap *empty_heap() {
    Heap *heap = malloc(sizeof(Heap));
    heap->size = 0;
    heap->first_root = NULL;
    heap->min = NULL;

    heap->stats.parent_child_swaps = 0;
    heap->stats.tree_merges = 0;
    heap->stats.root_removals = 0;
    heap->stats.root_inserts = 0;

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
    //free(removed_root);

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
    heap->stats.root_inserts++;

    merge(heap, small_heap);
    free(small_heap);
    return small_tree;
}

BinomialTree *merge_trees(BinomialTree *a, BinomialTree *b, Stats *stats) {
    if(a->key < b->key) {
        stats->tree_merges++;
        if (a->child)
            a->child->prev_sibling = b;
        b->next_sibling = a->child;
        b->prev_sibling = NULL;
        a->child = b;
        b->parent = a;
        a->order++;
        return a;
    }
    return merge_trees(b, a, stats);
}

Stats merge_stats(Stats a, Stats b) {
    Stats output;
    output.root_inserts = a.root_inserts + b.root_inserts;
    output.root_removals = a.root_removals + b.root_removals;
    output.parent_child_swaps = a.parent_child_swaps + b.parent_child_swaps;
    output.tree_merges = a.tree_merges + b.tree_merges;
    return output;
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
    output.stats = merge_stats(heap->stats, added->stats);

    BinomialTree *last_new_root = NULL;
    BinomialTree *tr_h = heap->first_root;
    BinomialTree *tr_a = added->first_root;
    BinomialTree *to_add = NULL;

    while (tr_h || tr_a) {
        to_add = NULL;
        // One tree is empty and don't have to merge with the last root added - add all the roots of the other to the end of the output root list
        if (tr_a == NULL && (last_new_root == NULL || last_new_root->order < tr_h->order)) {
            to_add = tr_h;
        }
        if (tr_h == NULL && (last_new_root == NULL || last_new_root->order < tr_a->order)) {
            to_add = tr_a;
        }
        if(to_add) {
            if (last_new_root == NULL && (last_new_root == NULL || last_new_root->order < tr_a->order)) {
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
            BinomialTree *next_a = tr_a->next_sibling;
            BinomialTree *next_h = tr_h->next_sibling;
            to_add = merge_trees(tr_h, tr_a, &output.stats);
            tr_h = next_h;
            tr_a = next_a;
        }

        if(last_new_root == NULL) {
            output.first_root = to_add;
            to_add->prev_sibling = NULL;
        } else if (to_add->order == last_new_root->order) {
            to_add = merge_trees(to_add, last_new_root, &output.stats);
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

void set_parent_of_siblings(BinomialTree *new_parent, BinomialTree *child) {

    while (child && child->prev_sibling) {
        child = child->prev_sibling;
    }
    if(new_parent) {
        new_parent->child = child;
    }
    while (child) {
        child->parent = new_parent;
        child = child->next_sibling;
    }
}

/*void swap_nodes(BinomialTree *a, BinomialTree *b) {
    BinomialTree old_a = *a;
    BinomialTree old_b = *b;
    a->next_sibling = old_b.next_sibling;
    if(a->next_sibling) {
        a->next_sibling->prev_sibling = a;
    }
    a->prev_sibling = old_b.prev_sibling;
    if(a->prev_sibling) {
        a->prev_sibling->next_sibling = a;
    }
    a->order = old_b.order;

    b->next_sibling = old_a.next_sibling;
    if (b->next_sibling) {
        b->next_sibling->prev_sibling = b;
    }
    b->prev_sibling = old_a.prev_sibling;
    if(b->prev_sibling) {
        b->prev_sibling->next_sibling = b;
    }
    b->order = old_a.order;

    if(a->parent == b) {
        a->parent = old_b.parent;
        b->parent = a;
        a->child = b;
        set_parent_of_siblings(b, old_a.child);
    } else if (b->parent == a) {
        b->parent = old_a.parent;
        a->parent = b;
        b->child = a;
        set_parent_of_siblings(a, old_b.child);
    } else {
        a->parent = old_b.parent;
        set_parent_of_siblings(a, old_b.child);
        b->parent = old_a.parent;
        set_parent_of_siblings(b, old_a.child);
    }
}*/

void swap_parent_child(BinomialTree *p, BinomialTree *ch) {
    BinomialTree old_p = *p;
    BinomialTree old_ch = *ch;

    ch->order = old_p.order;
    ch->parent = old_p.parent;
    if(old_p.parent && old_p.parent->child == p) {
        old_p.parent->child = ch;
    }

    ch->prev_sibling = old_p.prev_sibling;
    if(old_p.prev_sibling) {
        old_p.prev_sibling->next_sibling = ch;
    }

    ch->next_sibling = old_p.next_sibling;
    if(old_p.next_sibling) {
        old_p.next_sibling->prev_sibling = ch;
    }

    p->order = old_ch.order;

    if(old_p.child == ch) {
        ch->child = p;
    } else {
        ch->child = old_p.child;
    }

    p->prev_sibling = old_ch.prev_sibling;
    if(old_ch.prev_sibling) {
        old_ch.prev_sibling->next_sibling = p;
    }

    p->next_sibling = old_ch.next_sibling;
    if(old_ch.next_sibling) {
        old_ch.next_sibling->prev_sibling = p;
    }

    set_parent_of_siblings(ch, p);
    set_parent_of_siblings(p, old_ch.child);
};

void swap_nodes(BinomialTree *a, BinomialTree *b) {
    if(a->parent == b)
        return swap_parent_child(b, a);
    if(b->parent == a)
        return swap_parent_child(a, b);

    BinomialTree old_a = *a;
    BinomialTree old_b = *b;

    a->order = old_b.order;

    a->parent = old_b.parent;
    if(old_b.parent && old_b.parent->child == b) {
        old_b.parent->child = a;
    }

    a->prev_sibling = old_b.prev_sibling;
    if(old_b.prev_sibling) {
        old_b.prev_sibling->next_sibling = a;
    }

    a->next_sibling = old_b.next_sibling;
    if(old_b.next_sibling) {
        old_b.next_sibling->prev_sibling = a;
    }

    set_parent_of_siblings(a, old_b.child);

    b->order = old_a.order;

    b->parent = old_a.parent;
    if(old_a.parent && old_a.parent->child == a) {
        old_a.parent->child = b;
    }

    b->prev_sibling = old_a.prev_sibling;
    if(old_a.prev_sibling) {
        old_a.prev_sibling->next_sibling = b;
    }

    b->next_sibling = old_a.next_sibling;
    if(old_a.next_sibling) {
        old_a.next_sibling->prev_sibling = b;
    }

    set_parent_of_siblings(b, old_a.child);
}

void lower_key(BinomialTree *node, double new_key, Heap *heap) {
    node->key = new_key;
    BinomialTree *swap_with = node;
    while (swap_with->parent != NULL && swap_with->parent->key < new_key) {
        heap->stats.parent_child_swaps++;
        swap_with = swap_with->parent;
    }
    if (node != swap_with) {
        swap_nodes(node, swap_with);
    }
}

void dot_print_subtree(FILE *file, BinomialTree *tree) {
    if(tree == NULL) {
        return;
    }
    fprintf(file, "%d;\n", tree->data);
    if(tree->parent) {
        fprintf(file, "%d->%d [color=red];\n", tree->data, tree->parent->data);
    }
    if(tree->child) {
        fprintf(file, "%d->%d [color=black];\n", tree->data, tree->child->data);
        dot_print_subtree(file, tree->child);
    }
    if(tree->prev_sibling) {
        fprintf(file, "%d->%d [color=blue];\n", tree->data, tree->prev_sibling->data);
    }
    if(tree->next_sibling) {
        fprintf(file, "%d->%d [color=green];\n", tree->data, tree->next_sibling->data);
        dot_print_subtree(file, tree->next_sibling);
    }

}

void dot_printer(const char *filename, Heap *heap) {
    FILE *file = fopen(filename, "w");
    fprintf(file, "digraph G {\n");
    dot_print_subtree(file, heap->first_root);
    fprintf(file, "}\n");
    fclose(file);
}