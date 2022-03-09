#include "MSTApproximateTSPSolver.hpp"
#include "MinimumSpanningTree.hpp"
#include "Edge.hpp"

double MSTApproximateTSPSolver::approximate_shortest_cycle_cost(const std::vector<std::vector<double>>& weights)
{
	std::vector<Edge> edges = weights_matrix_to_edges(weights);
	size_t n_vertices = weights.size();
	std::vector<std::vector<size_t>> mst = build_mst(edges, n_vertices);
	std::vector<size_t> vertices_in_pre_order;
	traverse_tree_pre_order(mst, vertices_in_pre_order);
	double cycle_weight = 0;
	size_t prev = vertices_in_pre_order.back();
	for (size_t current : vertices_in_pre_order)
	{
		cycle_weight += weights[prev][current];
		prev = current;
	}
	return cycle_weight;
}

void MSTApproximateTSPSolver::traverse_tree_pre_order(
	const std::vector<std::vector<size_t>>& tree,
	std::vector<size_t>& vertices_in_pre_order,
	size_t parent,
	size_t vertex)
{
	vertices_in_pre_order.push_back(vertex);
	for (size_t neighbour : tree[vertex])
		if (neighbour != parent)
			traverse_tree_pre_order(tree, vertices_in_pre_order, vertex, neighbour);
}
