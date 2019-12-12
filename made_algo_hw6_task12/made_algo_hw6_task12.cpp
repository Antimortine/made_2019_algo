// Мосты.
// Ребро неориентированного графа называется мостом, если удаление этого ребра из графа увеличивает число компонент связности.
// Дан неориентированный граф, требуется найти в нем все мосты.

#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <stack>

struct edge_hash
{
	inline std::size_t operator()(const std::pair<int,int>& edge) const
	{
		return edge.first * 47 + edge.second;
	}
};

// Состояние нерекурсивной процедуры dfs, хранимое в стеке
struct dfs_state
{
	explicit dfs_state(int vertex, int parent, int current_edge_index, bool skipped_parent) :
		vertex(vertex), parent(parent), edge_index(current_edge_index), skipped_parent(skipped_parent)
	{
	}

	int vertex; // текущая рассматриваемая вершина
	int parent; // вершина, из которой пришли в vertex
	int edge_index; // индекс инцидентного ребра, на котором приостановили текущую итерацию алгоритма (равен -1, если это первое рассмотрение вершины vertex)
	bool skipped_parent; // равен true, если в процессе рассмотрения соседей был встречен parent. Повторное обнаружение parent означает, что ребро от него - кратное
};

// По условию задачи количество вершин может достигать 20000, следовательно, глубина рекурсии может превысить 1000,
// поэтому реализован нерекурсивный вариант обхода в глубину
void dfs(
	int start,
	int& time,
	std::vector<std::vector<int>>& graph,
	std::vector<bool>& visited,
	std::vector<int>& entry,
	std::vector<int>& lowest,
	std::unordered_set<std::pair<int, int>, edge_hash>& bridges
)
{
	std::stack<dfs_state> states;
	states.push(dfs_state(start, -1 , -1, false));

	while (!states.empty())
	{
		dfs_state state = states.top();
		states.pop();

		// Первое рассмотрение данной вершины
		if (state.edge_index == -1)
		{
			visited[state.vertex] = true;
			entry[state.vertex] = lowest[state.vertex] = time++;
		}
		// Иначе это аналог возврата из рекурсивного вызова
		else
		{
			int w = graph[state.vertex][state.edge_index];
			lowest[state.vertex] = std::min(lowest[state.vertex], lowest[w]);
			if (lowest[w] > entry[state.vertex])
			{
				bridges.insert(std::make_pair(state.vertex, w));
				bridges.insert(std::make_pair(w, state.vertex));
			}
		}
		state.edge_index++;

		// Возможно, для вершины state.vertex были рассмотрены не все соседи
		while (state.edge_index < graph[state.vertex].size())
		{
			int w = graph[state.vertex][state.edge_index];
			// Если при обходе мы второй раз наткнулись на parent, то это кратное ребро, и по нему можно пройти обратно
			if (w == state.parent && !state.skipped_parent)
			{
				state.skipped_parent = true;
				state.edge_index++;
			}
			else if (visited[w])
			{
				lowest[state.vertex] = std::min(lowest[state.vertex], entry[w]);
				state.edge_index++;
			}
			// Нашли непросмотренного соседа - приостанавливаем обработку текущей вершины, пока не закончим с соседом
			else
			{
				states.push(state); // Сохраняем текущее состояние
				states.push(dfs_state(w, state.vertex, -1, false)); // и начинаем обработку соседа
				break;
			}
		}
	}
}

std::unordered_set<std::pair<int, int>, edge_hash> find_bridges(std::vector<std::vector<int>>& graph)
{
	int time = 0;
	int n_vertices = graph.size() - 1;
	std::vector<bool> visited(n_vertices + 1, false);
	std::vector<int> entry(n_vertices + 1);
	std::vector<int> lowest(n_vertices + 1);
	std::unordered_set<std::pair<int, int>, edge_hash> bridges;
	for (int i = 1; i <= n_vertices; ++i)
		if (!visited[i])
			dfs(i, time, graph, visited, entry, lowest, bridges);
	return bridges;
}

int main()
{
	std::ifstream input("bridges.in");
	std::ofstream output("bridges.out");

	int n_vertices = 0;
	int n_edges = 0;
	input >> n_vertices >> n_edges;

	// Рёбра в оригинальном порядке
	std::vector<std::pair<int,int>> input_edges;

	// "Списки" смежности
	std::vector<std::vector<int>> graph(n_vertices + 1);
	for (int i = 1; i <= n_edges; ++i)
	{
		int u, v;
		input >> u >> v;
		input_edges.push_back(std::make_pair(u, v));
		graph[u].push_back(v);
		graph[v].push_back(u);
	}

	std::unordered_set<std::pair<int, int>, edge_hash> bridges = find_bridges(graph);
	int bridges_count = bridges.size() / 2;
	output << bridges_count << "\n";
	if (!bridges_count)
		return 0;

	for (int i = 1; i <= n_edges; ++i)
	{
		std::pair<int, int> edge = input_edges[i - 1];
		if (bridges.count(edge))
			output << i << " ";
	}
	
	return 0;
}
