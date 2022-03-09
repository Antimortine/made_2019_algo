#pragma once

#include <vector>

// Взвешенное ребро, соединяющее вершины left и right, имеющее вес weight
struct Edge
{
	explicit Edge(size_t left, size_t right, size_t weight);

	size_t left;
	size_t right;
	double weight;
};

bool compare_by_weight(const Edge& e1, const Edge& e2);

std::vector<Edge> weights_matrix_to_edges(const std::vector<std::vector<double>>& weights);
