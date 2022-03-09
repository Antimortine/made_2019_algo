#include "MinimumSpanningTree.hpp"
#include "DisjointSetUnion.hpp"
#include <algorithm>

std::vector<std::vector<size_t>> build_mst(std::vector<Edge>& edges, size_t n_vertices)
{
	// Сортируем рёбра по возрастанию их весов
	std::sort(edges.begin(), edges.end(), compare_by_weight);

	std::vector<std::vector<size_t>> mst_adjacency_lists(n_vertices, std::vector<size_t>());

	// Инициализируем систему непересекающихся множеств, помещая каждую вершину графа в своё множество
	DisjointSetUnion dsu(n_vertices);

	for (Edge e : edges)
	{
		size_t left_representative = dsu.find_representative(e.left);
		size_t right_representative = dsu.find_representative(e.right);
		if (left_representative == right_representative)
			continue; // Если ребро соединяет вершины из одного множества, то его добавление к остову приведёт к появлению цикла
		// Иначе добавим ребро к остову
		dsu.merge(left_representative, right_representative);
		mst_adjacency_lists[e.left].push_back(e.right);
		mst_adjacency_lists[e.right].push_back(e.left);
	}
	return mst_adjacency_lists;
}
