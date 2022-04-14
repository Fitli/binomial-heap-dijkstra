cmake_minimum_required(VERSION 3.17)
project(binomial_heap_dijkstra C)

set(CMAKE_C_STANDARD 99)

add_executable(binomial_heap_dijkstra main.c graph.c graph.h)
target_link_libraries(binomial_heap_dijkstra m)