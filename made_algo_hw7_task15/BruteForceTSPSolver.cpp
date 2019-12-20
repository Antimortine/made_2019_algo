#include "BruteForceTSPSolver.hpp"
#include <limits>
#include <utility>
#include <cassert>
#include <algorithm>

double BruteForceTSPSolver::get_shortest_cycle_cost(const std::vector<std::vector<double>>& weights)
{
	size_t n_vertices = weights.size();
	assert(n_vertices > 1);
	std::vector<bool> used_vertices(n_vertices, false);
	used_vertices[0] = true;
	size_t used_vertices_count = 1;
	return get_shortest_cycle_cost_recursive(
		weights,
		used_vertices,
		used_vertices_count,
		0,
		0
	);
}

double BruteForceTSPSolver::get_shortest_cycle_cost_recursive(
	const std::vector<std::vector<double>>& weights,
	std::vector<bool>& used_vertices,
	size_t used_vertices_count,
	double prefix_cost,
	size_t last_added_vertex)
{
	if (used_vertices_count == weights.size())
		return prefix_cost + weights[last_added_vertex][0];
	++used_vertices_count;
	double new_min_cost = std::numeric_limits<double>::max();
	for (size_t vertex = 1; vertex < weights.size(); ++vertex)
	{
		if (used_vertices[vertex])
			continue;
		used_vertices[vertex] = true;
		double new_prefix_cost = prefix_cost + weights[last_added_vertex][vertex];
		double result = get_shortest_cycle_cost_recursive(
			weights,
			used_vertices,
			used_vertices_count,
			new_prefix_cost,
			vertex);
		new_min_cost = std::min(new_min_cost, result);
		used_vertices[vertex] = false;
	}
	return new_min_cost;
}
