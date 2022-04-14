#include <stdio.h>
#include "graph.h"
#include "dijkstra.h"

int main() {
    Graph * gr = random_graph(10, 1);
    print_dot(gr);

    double len = path_lenght(gr, 1, 7);
    printf("len 1-5 = %lf\n", len);

    destroy_graph(gr);

    return 0;
}
