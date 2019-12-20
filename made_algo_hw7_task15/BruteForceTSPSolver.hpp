#pragma once

#include <vector>

// Находит точное решение неориентированной задачи коммивояжера путём полного перебора всех гамильтоновых циклов
class BruteForceTSPSolver
{
public:
	static double get_shortest_cycle_cost(const std::vector<std::vector<double>>& weights);
private:
	// Достраивает текущий путь до минимального гамильтонова цикла (среди циклов с данным префиксом) и возвращает его стоимость
	static double get_shortest_cycle_cost_recursive(
		const std::vector<std::vector<double>>& weights,
		std::vector<bool>& used_vertices,
		size_t used_vertices_count,
		double prefix_cost,
		size_t last_added_vertex);
};
