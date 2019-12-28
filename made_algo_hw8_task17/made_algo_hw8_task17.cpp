// Задача 17. Шаблон с ? (5 баллов)
// Шаблон поиска задан строкой длины m, в которой кроме обычных символов могут встречаться символы “?”.
// Найти позиции всех вхождений шаблона в тексте длины n.
// Каждое вхождение шаблона предполагает, что все обычные символы совпадают с соответствующими из текста,
// а вместо символа “?” в тексте встречается произвольный символ.
// Гарантируется, что сам “ ? ” в тексте не встречается.
// Время работы - O(n + m + Z), где Z - общее число вхождений подстрок шаблона “между вопросиками” в исходном тексте. (Ахо - Корасик)
// m ≤ 5000, n ≤ 2000000. Время : 10с, память 32Mb.

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <vector>
#include <cassert>

// Строит бор для заданного набора паттернов
// Позволяет находить все вхождения всех паттернов в строку с помощью алгоритма Ахо-Корасик
class Trie
{
public:
	struct Match
	{
		explicit Match(size_t start, size_t length, int pattern_index)
			: start(start), length(length), pattern_index(pattern_index) {};
		size_t start; // Индекс строки, начиная с котрого там читается паттерн
		size_t length; // Длина паттерна
		int pattern_index; // Номер паттерна (в порядке вызова add), который встретился в данной позиции
	};

	Trie();
	Trie(const Trie&) = delete;
	Trie(Trie&&) = delete;
	Trie& operator=(const Trie&) = delete;
	Trie& operator=(Trie&&) = delete;
	~Trie();

	// Добавляет паттерн в бор
	void add(const std::string& pattern);

	// Возвращает структуры Match для всех вхождений всех паттернов строку text
	std::vector<Match> find_all(const std::string& text);

private:
	struct Node
	{
		std::unordered_map<char, Node*> next; // Узлы, в которые можно непосредственно попасть из текущего, прочитав подходящий символ
		std::unordered_map<char, Node*> transition; // Узлы, в которые можно попасть либо напрямую, либо по суффиксным ссылкам
		Node* suffix_link = nullptr; // Суффиксная ссылка
		Node* terminal_suffix_link = nullptr; // Суффиксная ссылка в ближайшую терминальную вершину
		Node* parent = nullptr;
		char input_char = 0; // Символ, по которому можно попасть в данную вершину из parent
		int pattern_index = -1; // Номер паттерна (в порядке вызова add), который оканчивается в данной вершине
		size_t pattern_length = 0; // Длина паттерна, который оканчивается в данной вершине
	};

	Node* root;
	int patterns_count = 0; // Количество добавленных паттернов

	Node* get_suffix_link(Node* state); // Возвращает суффиксную ссылку узла state
	Node* get_terminal_suffix_link(Node* state); // Возвращает терминальную суффиксную ссылку узла state
	Node* transition(Node* state, char symbol); // Функция переходов автомата из состояния state по ребру symbol
	 // Функция выхода: добавляет в matches информацию обо всех вхождениях паттернов для вершины state и позиции в тексте text_index
	void out(Trie::Node* state, size_t text_index, std::vector<Trie::Match>& matches);
};

Trie::Trie()
{
	root = new Node();
	root->parent = root;
	root->suffix_link = root;
	root->terminal_suffix_link = root;
}

Trie::~Trie()
{
	std::queue<Node*> q;
	q.push(root);
	while (!q.empty())
	{
		Node* current = q.front();
		q.pop();
		for (auto next : current->next)
			q.push(next.second);
		delete current;
	}
}

void Trie::add(const std::string& pattern)
{
	assert(pattern.length() > 0);
	auto current = root;
	for (char symbol : pattern)
	{
		// Если такого перехода нет, то создадим его вместе с новой вершиной
		if (current->next.count(symbol) == 0)
		{
			Node* next = new Node();
			current->next[symbol] = next;
			current->transition[symbol] = next;
			next->parent = current;
			next->input_char = symbol;
			current = next;
		}
		else
			current = current->next[symbol];
	}
	assert(current->pattern_index == -1); // Паттерны должны быть уникальными
	current->pattern_index = patterns_count++;
	current->pattern_length = pattern.length();
}

std::vector<Trie::Match> Trie::find_all(const std::string& text)
{
	assert(text.length() > 0);
	assert(patterns_count > 0);
	std::vector<Match> matches;
	Node* state = root;

	for (int i = 0; i < text.length(); ++i)
	{
		char symbol = text[i];
		state = transition(state, symbol);
		out(state, i, matches);
	}

	return matches;
}

Trie::Node* Trie::get_suffix_link(Trie::Node* state)
{
	assert(state);
	if (!state->suffix_link)
	{
		if (state == root || state->parent == root)
			state->suffix_link = root;
		else
			state->suffix_link = transition(get_suffix_link(state->parent), state->input_char);
	}
	return state->suffix_link;
}

Trie::Node* Trie::get_terminal_suffix_link(Trie::Node* state)
{
	assert(state);
	if (!state->terminal_suffix_link)
	{
		state->terminal_suffix_link = get_suffix_link(state);
		while (state->terminal_suffix_link != root && state->terminal_suffix_link->pattern_index == -1)
			state->terminal_suffix_link = get_suffix_link(state->terminal_suffix_link);
	}
	return state->terminal_suffix_link;
}

Trie::Node* Trie::transition(Trie::Node* state, char symbol)
{
	assert(state);
	if (state->transition.count(symbol) == 0)
	{
		if (state->next.count(symbol) != 0)
			state->transition[symbol] = state->next[symbol]; // Если есть прямой переход, то идём по нему
		else if (state == root)
			state->transition[symbol] = root;
		else
			state->transition[symbol] = transition(get_suffix_link(state), symbol);
	}
	return state->transition[symbol];
}

void Trie::out(Trie::Node* state, size_t text_index, std::vector<Trie::Match>& matches)
{
	assert(state);
	if (state->pattern_index == -1)
		state = get_terminal_suffix_link(state);
	while (state != root)
	{
		size_t length = state->pattern_length;
		Match match(text_index - length + 1, length, state->pattern_index);
		matches.emplace_back(match);
		state = get_terminal_suffix_link(state);
	}
}

// Разбивает паттерн на подстроки, каждая из которых либо не содержит символов "?", либо состоит только из них
std::vector<std::string> split_pattern(const std::string& pattern)
{
	assert(pattern.length() > 0);
	std::vector<std::string> parts;
	size_t substr_start = 0;
	bool is_prev_question = pattern[0] == '?';

	for (size_t i = 1; i < pattern.length(); ++i)
	{
		bool is_question = pattern[i] == '?';
		if (is_prev_question != is_question)
		{
			parts.emplace_back(pattern.substr(substr_start, i - substr_start));
			substr_start = i;
			is_prev_question = is_question;
		}
	}
	parts.emplace_back(pattern.substr(substr_start, pattern.length() - substr_start));
	return parts;
}

// Часть паттерна между символами "?"
struct SubPattern
{
	explicit SubPattern(int index, size_t length, size_t questions_left)
		: index(index), length(length), questions_left(questions_left) {};
	explicit SubPattern(int index, size_t length, size_t questions_left, size_t questions_right)
		: index(index), length(length), questions_left(questions_left), questions_right(questions_right){};
	int index; // Индекс уникального субпаттерна в паттерне
	size_t length; // Количество символов в субпаттерне
	size_t questions_left = 0; // Количество символов "?" слева от субпаттерна в паттерне
	size_t questions_right = 0; // Количество символов "?" справа от субпаттерна в паттерне
};

// Принимает на вход паттерн, возможно, содержащий символы "?"
// Возвращает:
// - subpatterns - вектор структур SubPattern, описывающих каждую часть патерна между символами "?"
// - unique_substrings - вектор уникальных частей патерна между символами "?" в порядке их первого появления в паттерне
void parse_pattern(const std::string& pattern, std::vector<SubPattern>& subpatterns, std::vector<std::string>& unique_substrings)
{
	std::map<std::string, size_t> substrings_to_indices; // Сопоставляет каждому субпаттерну его номер
	std::vector<std::string> parts = split_pattern(pattern);

	size_t questions_left = 0;
	size_t i = 0;
	std::string& part = parts[i];

	if (part[0] == '?')
	{
		questions_left = part.length();
		// Если паттерн состоит целиком из вопросов
		if (parts.size() == 1)
		{
			subpatterns.emplace_back(SubPattern(-1, 0, questions_left, 0));
			return;
		}
		++i;
	}

	while (i < parts.size())
	{
		part = parts[i];
		size_t pattern_index = 0;
		// Если данный субпаттерн ранее не встречался, назначим ему новый номер
		if (substrings_to_indices.find(part) == substrings_to_indices.end())
		{
			pattern_index = substrings_to_indices.size();
			substrings_to_indices[part] = pattern_index;
			unique_substrings.emplace_back(part);
		}
		else
		{
			pattern_index = substrings_to_indices[part];
		}
		SubPattern subpattern(static_cast<int>(pattern_index), part.length(), questions_left);
		++i; // Пытаемся обработать следующий блок вопросов, если он есть
		if (i < parts.size())
		{
			questions_left = parts[i].length();
			subpattern.questions_right = questions_left;
		}
		subpatterns.emplace_back(subpattern);
		++i; // И переходим к следующему паттерну
	}
	return;
}

// Разбивает паттерн на субпаттерны (их описание помещается в subpatterns),
// применяет алгоритм Ахо-Корасик и агрегирует результаты в виде двух коллекций matches_map и matches_starts
// (описаны внутри find_all)
void find_partial_matches(
	const std::string& pattern,
	const std::string& text,
	std::vector<SubPattern>& subpatterns,
	std::map<std::pair<size_t, int>, Trie::Match>& matches_map,
	std::set<size_t>& matches_starts)
{
	std::vector<std::string> unique_substrings;
	parse_pattern(pattern, subpatterns, unique_substrings);

	// Если паттерн состоит только из знаков "?"
	if (subpatterns[0].index == -1)
		return;

	// Создаём автомат Ахо-Корасик
	Trie trie;
	for (std::string& substr : unique_substrings)
		trie.add(substr);

	// Находим вхождения всех субпаттернов
	std::vector<Trie::Match> matches = trie.find_all(text);

	unique_substrings.clear();
	std::vector<std::string>().swap(unique_substrings);

	if (matches.size() < subpatterns.size())
		return;

	for (Trie::Match& match : matches)
	{
		matches_map.emplace(std::make_pair(match.start, match.pattern_index), match);
		if (matches_starts.find(match.start) == matches_starts.end())
			matches_starts.insert(match.start);
	}
}

// Возвращает индексы всех вхождений паттерна pattern (возможно, содержащего символы "?") в строку text
std::vector<size_t> find_all(const std::string& pattern, const std::string& text)
{
	assert(pattern.length() > 0);
	assert(text.length() > 0);
	std::vector<size_t> indices;

	if (pattern.length() > text.length())
		return indices;

	std::vector<SubPattern> subpatterns;

	// Первый элемент ключа - позиция в тексте
	// Второй элемент - индекс (идентификатор) уникального субпаттерна в оригинальном шаблоне
	std::map<std::pair<size_t, int>, Trie::Match> matches_map;

	// Все позиции текста, в которых есть вхождения субпаттернов
	std::set<size_t> matches_starts;

	find_partial_matches(
		pattern,
		text,
		subpatterns,
		matches_map,
		matches_starts);

	// Если паттерн состоит только из знаков "?"
	if (subpatterns[0].index == -1)
	{
		for (size_t i = 0; i <= text.length() - pattern.length(); ++i)
			indices.push_back(i);
		return indices;
	}

	if (matches_starts.size() == 0)
		return indices;

	// Теперь пытаемся сопоставить матчи с оригинальным паттерном (должен совпадать порядок + расстояние между ними)
	size_t leading_questions = subpatterns[0].questions_left; // Количество начальных символов "?" в паттерне

	for (size_t text_index : matches_starts)
	{
		// Пропустим вхождения, слева от которых меньше символов, чем знаков "?" в начале паттерна
		if (text_index < leading_questions)
			continue;

		size_t match_must_start = text_index; // Позиция в тексте, в которой должен начинаться матч
		size_t matches_count = 0;
		for (SubPattern& subpattern : subpatterns)
		{
			// Если в данной позиции нет матча, соответствующего текущему субпаттерну, то нужно искать совпадение дальше
			if (matches_map.find(std::make_pair(match_must_start, subpattern.index)) == matches_map.end())
				break;

			Trie::Match& match = matches_map.at(std::make_pair(match_must_start, subpattern.index));

			// Расстояние между матчами должно совпадать с количеством вопросов между субпаттернами
			match_must_start = match.start + match.length + subpattern.questions_right;

			// Случай, когда справа от конца текущего матча меньше символов, чем знаков "?" следом за ним в паттерне
			if (match_must_start > text.length())
				break;

			++matches_count;
		}
		if (matches_count != subpatterns.size())
			continue;

		// Ура, нашли вхождение всего паттерна
		indices.emplace_back(text_index - leading_questions);
	}

	return indices;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string pattern;
	std::string text;

	std::cin >> pattern >> text;

	//pattern = "dabce?abc?bc?bc?bc?abc";
	//text = "bcdabceaabcabcabcabcaabc";

	std::vector<size_t> indices = find_all(pattern, text);
	for (size_t index : indices)
		std::cout << index << " ";

	std::cout.flush();
	return 0;
}
