#include <stdio.h>
#include "graph.h"

int main() {
    Graph * gr = random_graph(10, 1);
    print_dot(gr);
    return 0;
}
