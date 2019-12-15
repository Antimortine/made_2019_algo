// 13. «Пятнашки»
// Написать алгоритм для решения игры в “пятнашки”
// Достаточно найти хотя бы какое-то решение. Число перемещений костяшек не обязано быть минимальным.

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <unordered_map>

// Состояние игрового поля
struct position
{
	std::vector<char> cells; // Значения ячеек при построчном обходе слева направо сверху вниз
	char empty_index; // Индекс пустой ячейки в векторе cells

	bool is_final() const; // Является ли позиция искомой
	bool is_solvable() const; // Достижима ли финальная позиция из текущей
	std::vector<position> get_nearest_positions() const; // Возвращает позиции, достижимые из текущей за один ход

	bool operator==(const position& other) const;
	bool operator!=(const position& other) const { return !operator==(other); }

	static const position FINAL;
};

const position position::FINAL{ {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, 15 };

namespace std
{
	template <>
	struct hash<position>
	{
		size_t operator()(const position& key) const
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
	return operator==(position::FINAL);
}

bool position::is_solvable() const
{
	// Считаем количество инверсий без учёта пустой ячейки
	int inversions_count = 0;
	for (int i = 0; i < cells.size() - 1; ++i)
	{
		if (i == empty_index) continue;
		for (int j = i + 1; j < cells.size(); ++j)
		{
			if (j == empty_index) continue;
			if (cells[i] > cells[j])
				++inversions_count;
		}
	}
	int empty_row = empty_index / 4; // Определяем индекс строки, в которой находится пустая ячейка
	int invariant = (inversions_count + empty_row) & 1;
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

// Возвращает метку движения, позволяющего попасть из позиции from в to
char get_move_symbol(const position& from, const position& to)
{
	char empty_diff = to.empty_index - from.empty_index;
	switch (empty_diff)
	{
	case 1:
		return 'L'; // Ноль вправо -> фишка влево
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

bool BFS(const position& start, std::unordered_map<position, position>& parents)
{
	std::queue<position> positions_queue;
	positions_queue.push(start);
	while (!positions_queue.empty())
	{
		position current = positions_queue.front();
		positions_queue.pop();
		for (auto sibling : current.get_nearest_positions())
		{
			if (parents.count(sibling))
			{
				continue;
			}
			parents[sibling] = current;
			if (sibling.is_final())
			{
				return true;
			}
			positions_queue.push(sibling);
		}
	}
	return false;
}

// Восстанавливает последовательность ходов, ведущих от start к финальному состоянию
std::vector<char> build_moves_sequence(const position& start,
	const std::unordered_map<position, position>& parents)
{
	std::vector<char> moves;

	position current = position::FINAL;
	while (current != start)
	{
		const position parent = parents.at(current);
		moves.push_back(get_move_symbol(parent, current));
		current = parent;
	}
	// Развернем moves, так как собирали его с конца
	std::reverse(moves.begin(), moves.end());
	return moves;
}

// Если задача разрешима, возвращает число ходов и обозначения самих ходов
// Иначе возвращает -1 и пустой вектор
std::pair<int, std::vector<char>> solve_puzzle15(const position& start)
{
	if (!start.is_solvable())
		return std::make_pair(-1, std::vector<char>());

	// Запустим BFS, запоминая предков всех пройденных позиций.
	std::unordered_map<position, position> parents;
	BFS(start, parents);
	auto path = build_moves_sequence(start, parents);
	return std::make_pair(path.size(), path);
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	//position start{ {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15}, 14 };
	//position start{ {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 14, 0}, 15 };
	position start{ {1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 13, 14, 15, 12}, 3 };
	//3
	//U U U

	const auto result = solve_puzzle15(start);
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
