#pragma once

#include <vector>

// Находит приближённое решение метрической неориентированной задачи коммивояжера путём построения минимального остовного дерева алгоритмом Крускала
class MSTApproximateTSPSolver
{
public:
	static double approximate_shortest_cycle_cost(const std::vector<std::vector<double>>& weights);
private:
	// Обходит дерево в порядке pre-order и возвращает узлы в порядке их посещения
	static void traverse_tree_pre_order(
		const std::vector<std::vector<size_t>>& tree,
		std::vector<size_t>& vertices_in_pre_order,
		size_t parent = 0,
		size_t vertex = 0);
};
