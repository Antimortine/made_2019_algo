#pragma once

#include "Edge.hpp"

// Строит минимальное остовное дерева (в виде списков смежности) для графа, заданного рёбрами edges и имеющего n_vertices вершин
// Используется алгоритм Крускала
std::vector<std::vector<size_t>> build_mst(std::vector<Edge>& edges, size_t n_vertices);
