#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "dijkstra.h"

int main() {
    printf("seed, num_nodes,num_edges,c,pops,inserts,decreases,tree_merges,root_removals,root_inserts,parent_child_swaps,find_min_iters\n");
    for (int n = 1; n < 10000; n+=50) {
        for (int c = 1; c <= 5; c++) {
            for(int i = 0; i < 10; i++) {
                srand(i);
                Graph *gr = random_graph(n, c);
                printf("%d,", i);

                fflush(stdout);
                double len = path_lenght(gr, 0, 0);

                destroy_graph(gr);
            }
        }
    }
    return 0;
}
