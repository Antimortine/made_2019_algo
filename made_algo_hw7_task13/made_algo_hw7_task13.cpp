// 13. «Пятнашки»
// Написать алгоритм для решения игры в “пятнашки”
// Достаточно найти хотя бы какое-то решение. Число перемещений костяшек не обязано быть минимальным.

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <climits>
#include <unordered_set>

constexpr int field_size = 16;

// Состояние игрового поля
struct position
{
	std::vector<char> cells; // Значения ячеек при построчном обходе слева направо сверху вниз
	char empty_index = field_size-1; // Индекс пустой ячейки в векторе cells
	int priority = INT_MAX; // Приоритет для хранения в MinHeap

	bool is_final() const; // Является ли позиция искомой
	bool is_solvable() const; // Достижима ли финальная позиция из текущей
	std::vector<position> get_nearest_positions() const; // Возвращает позиции, достижимые из текущей за один ход

	bool operator==(const position& other) const;
	bool operator!=(const position& other) const { return !operator==(other); }

	static const position final;
};

const position position::final{ {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, field_size-1 };

namespace std
{
	// Полиномиальный хэш для хранения позиций в unordered_set/unordered_map
	template <>
	struct hash<position>
	{
		size_t operator()(const position& key) const noexcept
		{
			size_t value = 0;
			for (auto cell : key.cells)
			{
				value = 37 * value + static_cast<size_t>(cell);
			}
			return value;
		}
	};
}

bool position::is_final() const
{
	return operator==(final);
}

// Количество инверсий без учёта пустой ячейки при построчном обходе слева направо сверху вниз
int get_row_order_inversions_count(const position& pos)
{
	int inversions_count = 0;
	for (int i = 0; i < field_size - 1; ++i)
	{
		if (i == pos.empty_index) continue;
		for (int j = i + 1; j < field_size; ++j)
		{
			if (j == pos.empty_index) continue;
			if (pos.cells[i] > pos.cells[j])
				++inversions_count;
		}
	}
	return inversions_count;
}

const int column_order_indices[] = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };

// Количество инверсий без учёта пустой ячейки при обходе по столбцам сверху вниз слева направо
int get_column_order_inversions_count(const position& pos)
{
	int inversions_count = 0;
	for (int i = 0; i < field_size - 1; ++i)
	{
		const int i_position = column_order_indices[i];
		if (i_position == pos.empty_index) continue;
		for (int j = i + 1; j < field_size; ++j)
		{
			const int j_position = column_order_indices[j];
			if (j_position == pos.empty_index) continue;
			if (pos.cells[i_position] > pos.cells[j_position])
				++inversions_count;
		}
	}
	return inversions_count;
}

bool position::is_solvable() const
{
	const int inversions_count = get_row_order_inversions_count(*this);
	const int empty_row = empty_index / 4; // Определяем индекс строки, в которой находится пустая ячейка
	const int invariant = (inversions_count + empty_row) & 1;
	return invariant;
}

std::vector<position> position::get_nearest_positions() const
{
	std::vector<position> neighbours;
	// Если пустая ячейка не в последней строке, то её можно сдвинуть вниз
	if (empty_index < 12)
	{
		position neighbour = *this;
		std::swap(neighbour.cells[empty_index], neighbour.cells[empty_index + 4]);
		neighbour.empty_index += 4;
		neighbours.emplace_back(neighbour);
	}
	// Если пустая ячейка не в первой строке, то её можно сдвинуть вверх
	if (empty_index >= 4)
	{
		position neighbour = *this;
		std::swap(neighbour.cells[empty_index], neighbour.cells[empty_index - 4]);
		neighbour.empty_index -= 4;
		neighbours.emplace_back(neighbour);
	}
	// Если пустая ячейка не в первом столбце, то её можно сдвинуть влево
	if (empty_index % 4 != 0)
	{
		position neighbour = *this;
		std::swap(neighbour.cells[empty_index], neighbour.cells[empty_index - 1]);
		neighbour.empty_index--;
		neighbours.emplace_back(neighbour);
	}
	// Если пустая ячейка не в последнем столбце, то её можно сдвинуть вправо
	if (empty_index % 4 != 3)
	{
		position neighbour = *this;
		std::swap(neighbour.cells[empty_index], neighbour.cells[empty_index + 1]);
		neighbour.empty_index++;
		neighbours.emplace_back(neighbour);
	}
	return neighbours;
}

bool position::operator==(const position& other) const
{
	for (size_t i = 0; i < cells.size(); ++i)
		if (cells[i] != other.cells[i]) return false;
	return true;
}

// Сумма манхэттенских расстояний от каждой ячейки до её положения в финальном состоянии
// Является допустимой эвристикой
int manhattan_distance_to_final(const position& pos)
{
	int distance = 0;
	for (int i = 0; i < field_size; ++i)
	{
		if (i == pos.empty_index)
			continue;
		const int current_row = i / 4;
		const int final_row = (pos.cells[i] - 1) / 4;
		const int current_column = i % 4;
		const int final_column = (pos.cells[i] - 1) % 4;
		distance += std::abs(final_row - current_row) + std::abs(final_column - current_column);
	}
	return distance;
}

// Оценка минимального количества ходов на основе инверсий при чтении по строкам и по столбцам
// Является допустимой эвристикой
int invert_distance(const position& pos)
{
	const int row_order_inversions = get_row_order_inversions_count(pos);
	const int column_order_inversions = get_column_order_inversions_count(pos);
	// Ходы по вертикали уменьшают число инверсий по строкам не более, чем на 3
	// Аналогично с горизонтальными ходами и инверсиями по столбцам
	return row_order_inversions / 3 + row_order_inversions % 3 + column_order_inversions / 3 + column_order_inversions % 3;
}

int heuristic(const position& pos)
{
	return std::max(manhattan_distance_to_final(pos), invert_distance(pos));
}

// Возвращает метку движения, позволяющего попасть из позиции from в to
char get_move_symbol(const position& from, const position& to)
{
	// Перемещение пустой ячейки
	const int empty_diff = to.empty_index - from.empty_index;
	switch (empty_diff)
	{
	case 1:
		return 'L';
	case -1:
		return 'R';
	case 4:
		return 'U';
	case -4:
		return 'D';
	default:
		throw std::invalid_argument("Positions must be adjacent");
	}
}

// Компаратор для очереди с приоритетами
// Будем извлекать позиции с минимальным приоритетом
bool priority_comparator (const position& p1, const position& p2)
{
	return p1.priority > p2.priority;
}

// Алгоритм A*. В основе лежит алгоритм Дейкстры, не использующий decrease_key.
// Вместо этого извлечённые из очереди вершины помещаются в unordered_set, а при повторном извлечении игнорируются.
// Это не изменяет асимптотику алгоритма: под знаком логарифма оказывается |E| вместо |V|, O(log|E|)=O(log|V|^2)=O(2log|V|)=O(log|V|)
// Значение heuristic_weight=1 сохраняет эвристику допустимой и позволяет найти кратчайший путь
// Значения heuristic_weight>1 могут нарушить допустимость эвристики. Это может привести к нахождению неоптимлаьного пути,
// но, как правило, путь будет найден быстрее
void a_star(position& start, std::unordered_map<position, position>& parents, int heuristic_weight)
{
	std::unordered_map<position, int> distance; // Расстояния от стартовой вершины по числу рёбер
	std::unordered_set<position> extracted; // Была ли позиция извлечена из очереди (т.е. вычислено ли для неё кратчайшее расстояние от старта)
	distance[start] = 0;
	
	std::priority_queue<position, std::vector<position>, decltype(&priority_comparator)> positions_queue(priority_comparator);
	start.priority = 0;
	positions_queue.push(start);
	
	while (!positions_queue.empty())
	{
		position current = positions_queue.top();
		positions_queue.pop();
		if (extracted.count(current)) // Позиция извлекалась ранее
			continue;
		if (current.is_final())
			return;
		
		extracted.insert(current);
		
		for (position& neighbour : current.get_nearest_positions())
		{
			if (distance.count(neighbour) == 0 || distance[neighbour] > distance[current] + 1)
			{
				parents[neighbour] = current;
				distance[neighbour] = distance[current] + 1;
				neighbour.priority = distance[neighbour] + heuristic_weight * heuristic(neighbour);
				positions_queue.push(neighbour);
			}
		 }
	}
}

// Восстанавливает последовательность ходов, ведущих от start к финальному состоянию
std::vector<char> build_moves_sequence(const position& start,
	const std::unordered_map<position, position>& parents)
{
	std::vector<char> moves;

	position current = position::final;
	while (current != start)
	{
		const position& parent = parents.at(current);
		moves.push_back(get_move_symbol(parent, current));
		current = parent;
	}
	// Развернем moves, так как собирали его с конца
	std::reverse(moves.begin(), moves.end());
	return moves;
}

// Если задача разрешима, возвращает число ходов и обозначения самих ходов
// Иначе возвращает -1 и пустой вектор
std::pair<int, std::vector<char>> solve_puzzle15(position& start, int heuristic_weight = 1)
{
	if (!start.is_solvable())
		return std::make_pair(-1, std::vector<char>());

	// Запустим A*, запоминая предков всех пройденных позиций.
	std::unordered_map<position, position> parents;
	a_star(start, parents, heuristic_weight);
	auto path = build_moves_sequence(start, parents);
	return std::make_pair(path.size(), path);
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::vector<char> cells(field_size);
	char empty_position = 0;
	for (char i = 0; i < field_size; ++i)
	{
		int cell;
		std::cin >> cell;
		cells[i] = static_cast<char>(cell);
		if (cell == 0)
			empty_position = i;
	}
	position start{ cells, empty_position };

	const auto result = solve_puzzle15(start, 4);
	std::cout << result.first << "\n";

	if (result.first == -1)
	{
		std::cout.flush();
		return 0;
	}
	for (const auto move : result.second)
		std::cout << move << " ";

	std::cout.flush();
	return 0;
}
