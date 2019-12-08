// 11_2. ���������� ��������� �����
// ��� ������������ ����������������� ����. � ����� ����� ���� ��������� ���������� ����� ����� ������-�� ���������.
// ������� ���������� ��������� ���������� ����� ����� ��������� ���������. ��������� ��������� O(V + E).
// ����: v: ��� - �� ������(����. 50000), n : ��� - �� �����(����. 200000), n ��� �������� ������, ���� ������ v, w ��� �������.
// ����� : ���������� ���������� ����� �� v � w.

#include <iostream>
#include <vector>
#include <queue>
#include <climits>

// ��������� ���������� ���������� ����� �� ������� start �� ������� end ������� � ������
int count_shortest_paths(std::vector<std::vector<int>>& edges, int start, int end, int n_vertices)
{
	// ������ ���������� ���������� �� ������� start �� ���� ���������
	std::vector<int> shortest_distances(n_vertices, INT_MAX);
	shortest_distances[start] = 0;

	// ������ ���������� ���������� ����� �� ������� start �� ���� ���������
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

