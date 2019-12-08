// 11_2. Количество различных путей
// Дан невзвешенный неориентированный граф. В графе может быть несколько кратчайших путей между какими-то вершинами.
// Найдите количество различных кратчайших путей между заданными вершинами. Требуемая сложность O(V + E).
// Ввод: v: кол - во вершин(макс. 50000), n : кол - во ребер(макс. 200000), n пар реберных вершин, пара вершин v, w для запроса.
// Вывод : количество кратчайших путей от v к w.

#include <iostream>
#include <vector>
#include <queue>
#include <climits>

// Вычисляет количество кратчайших путей от вершины start до вершины end обходом в ширину
int count_shortest_paths(std::vector<std::vector<int>>& edges, int start, int end, int n_vertices)
{
	// Хранит кратчайшие расстояния от вершины start до всех остальных
	std::vector<int> shortest_distances(n_vertices, INT_MAX);
	shortest_distances[start] = 0;

	// Хранит количество кратчайших путей от вершины start до всех остальных
	std::vector<int> n_shortest_paths(n_vertices, 0);
	n_shortest_paths[start] = 1;

	std::queue<int> q;
	q.push(start);

	while (!q.empty())
	{
		int v = q.front();
		q.pop();
		for (int w : edges[v])
		{
			if (shortest_distances[w] > shortest_distances[v] + 1)
			{
				shortest_distances[w] = shortest_distances[v] + 1;
				n_shortest_paths[w] = n_shortest_paths[v];
				q.push(w);
			}
			else if (shortest_distances[w] == shortest_distances[v] + 1)
			{
				n_shortest_paths[w] += n_shortest_paths[v];
			}
		}
	}
	return n_shortest_paths[end];
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	int n_vertices = 0;
	int n_edges = 0;
	std::cin >> n_vertices >> n_edges;

	std::vector<std::vector<int>> edges(n_vertices);
	for (int i = 0; i < n_edges; ++i)
	{
		int u, w;
		std::cin >> u >> w;
		edges[u].push_back(w);
		edges[w].push_back(u);
	}

	int start = 0;
	int end = 0;
	std::cin >> start >> end;

	int n_shortest_paths = count_shortest_paths(edges, start, end, n_vertices);

	std::cout << n_shortest_paths;
	std::cout.flush();
	return 0;
}

