// 14. «MST». Дан неориентированный связный граф. Требуется найти вес минимального остовного дерева в этом графе.
// Вариант 2. С помощью алгоритма Крускала.

#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>

// Взвешенное ребро, соединяющее вершины left и right, имеющее вес weight
struct edge
{
	explicit edge(size_t left, size_t right, size_t weight) :
		left(left), right(right), weight(weight)
	{
	}

	size_t left;
	size_t right;
	size_t weight;
};

bool compare_by_weight(const edge& e1, const edge& e2)
{
	return e1.weight < e2.weight;
}

// Система непересекающихся множеств
class DSU
{
public:
	explicit DSU(size_t size);

	size_t find_representative(size_t element); // Находит представителя множества, в котором находится element
	void merge(size_t left_representative, size_t right_representative); // Объединяет два множества, заданных их представителями

private:
	std::vector<size_t> parent; // Индекс родительского элемента в дереве
	std::vector<size_t> rank; // Оценка сверху высоты дерева
};

DSU::DSU(size_t size) :
	parent(size),
	rank(size, 0)
{
	for (size_t i = 0; i < parent.size(); ++i)
		parent[i] = i;
}

size_t DSU::find_representative(size_t element)
{
	if (parent[element] == element)
		return element;
	return parent[element] = find_representative(parent[element]);
}

void DSU::merge(size_t left_representative, size_t right_representative)
{
	assert(left_representative != right_representative);
	if (rank[left_representative] == rank[right_representative])
	{
		parent[right_representative] = left_representative;
		++rank[left_representative];
	}
	else if (rank[left_representative] > rank[right_representative])
	{
		parent[right_representative] = left_representative;
	}
	else
	{
		parent[left_representative] = right_representative;
	}
}

// Возвращает вес минимального остовного дерева для графа, заданного рёбрами edges и имеющего n_vertices вершин
// Используется алгоритм Крускала
size_t find_mst_weight(std::vector<edge>& edges, size_t n_vertices)
{
	// Сортируем рёбра по возрастанию их весов
	std::sort(edges.begin(), edges.end(), compare_by_weight);

	// Инициализируем систему непересекающихся множеств, помещая каждую вершину графа в своё множество
	DSU dsu(n_vertices);

	size_t mst_weight = 0;

	for (edge e : edges)
	{
		size_t left_representative = dsu.find_representative(e.left);
		size_t right_representative = dsu.find_representative(e.right);
		if (left_representative == right_representative)
			continue; // Если ребро соединяет вершины из одного множества, то его добавление к остову приведёт к появлению цикла
		// Иначе добавим ребро к остову
		dsu.merge(left_representative, right_representative);
		mst_weight += e.weight;
	}

	return mst_weight;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	size_t n_vertices = 0;
	size_t n_edges = 0;
	std::cin >> n_vertices >> n_edges;
	++n_vertices; // Вершины нумеруются с единицы

	// Считываем рёбра с их весами
	std::vector<edge> edges;
	for (size_t i = 0; i < n_edges; ++i)
	{
		size_t left, right, weight;
		std::cin >> left >> right >> weight;
		edges.push_back(edge(left, right, weight));
	}

	size_t mst_weight = find_mst_weight(edges, n_vertices);
    std::cout << mst_weight;
	std::cout.flush();
	return 0;
}
