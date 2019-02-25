cmake_minimum_required(VERSION 3.13)
project(CitationGraph)

set(CMAKE_CXX_STANDARD 17)

add_executable(citation_graph citation_graph_example.cc citation_graph.h)