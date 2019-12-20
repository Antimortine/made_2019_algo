#include "Edge.hpp"

Edge::Edge(size_t left, size_t right, size_t weight) :
	left(left), right(right), weight(weight)
{}

bool compare_by_weight(const Edge& e1, const Edge& e2)
{
	return e1.weight < e2.weight;
}

std::vector<Edge> weights_matrix_to_edges(const std::vector<std::vector<double>>& weights)
{
	size_t n_vertices = weights.size();
	std::vector<Edge> edges;
	for (size_t i = 0; i < n_vertices; ++i)
		for (size_t j = i + 1; j < n_vertices; ++j)
			edges.push_back(Edge(i, j, weights[i][j]));
	return edges;
}
