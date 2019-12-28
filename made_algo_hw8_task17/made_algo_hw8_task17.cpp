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
#include <string_view>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
#include <cassert>
#include <cstdint>

// Строит бор для заданного набора паттернов
// Позволяет находить все вхождения всех паттернов в строку с помощью алгоритма Ахо-Корасик
class Trie
{
public:
	struct Match
	{
		explicit Match(size_t start, int16_t pattern_index)
			: start(start), pattern_index(pattern_index) {};

		bool operator==(const Match& other) const
		{
			return start == other.start && pattern_index == other.pattern_index;
		}

		bool operator<(const Match& other) const
		{
			return start < other.start ||
				(start == other.start && pattern_index < other.pattern_index);
		}

		size_t start; // Индекс строки, начиная с котрого там читается паттерн
		int16_t pattern_index; // Номер паттерна (в порядке вызова add), который встретился в данной позиции
	};

	Trie();
	Trie(const Trie&) = delete;
	Trie(Trie&&) = delete;
	Trie& operator=(const Trie&) = delete;
	Trie& operator=(Trie&&) = delete;
	~Trie() = default;

	// Добавляет паттерн в бор
	void add(const std::string_view& pattern);

	// Возвращает структуры Match для всех вхождений всех паттернов строку text
	std::set<Trie::Match> find_all(const std::string& text);

private:
	int16_t nodes_count = 1;

	std::vector<int16_t> pattern_lengths; // Длины паттернов (в порядке вызова add)
	std::vector<int16_t> parents; // Индексы родительских узлов
	std::vector<char> input_chars; // Для каждого узла хранит символ, по которому в него можно попасть из родительского
	std::vector<int16_t> suffix_links; // Суффиксные ссылки
	std::vector<int16_t> terminal_links; // Суффиксные ссылки в ближайшую терминальную вершину

	// Функция перехода
	// Паре (индекс узла, символ) сопаставляет индекс узла, в который можно попасть либо напрямую, либо по суффиксным ссылкам
	std::map<std::pair<char, int16_t>, int16_t> transitions;

	// Сопоставляет номерам узлов индексы паттернов (в порядке вызова add), которые там заканчиваются
	std::map<int16_t, int16_t> terminals;

	int16_t get_suffix_link(int16_t state_index); // Возвращает суффиксную ссылку
	int16_t get_terminal_suffix_link(int16_t state_index); // Возвращает терминальную суффиксную ссылку
	int16_t transition(int16_t state_index, char symbol); // Функция переходов автомата из состояния state_index по ребру symbol

	// Функция выхода: добавляет в matches информацию обо всех вхождениях паттернов для вершины nodes[state_index]
	// и позиции в тексте text_index
	void out(int16_t state_index, size_t text_index, std::set<Trie::Match>& matches);
};

Trie::Trie()
{
	parents.push_back(0);
	input_chars.push_back(0);
	suffix_links.push_back(0);
	terminal_links.push_back(0);
}

void Trie::add(const std::string_view& pattern)
{
	assert(pattern.length() > 0);
	int current_node_index = 0;
	for (char symbol : pattern)
	{
		// Если такого перехода нет, то создадим его вместе с новой вершиной
		std::pair<char, int16_t> key = std::make_pair(symbol, current_node_index);
		if (transitions.count(key) == 0)
		{
			transitions[key] = nodes_count;
			parents.push_back(current_node_index);
			input_chars.push_back(symbol);
			suffix_links.push_back(-1);
			terminal_links.push_back(-1);
			current_node_index = nodes_count++;
		}
		else
			current_node_index = transitions[key];
	}
	assert(terminals.count(current_node_index) == 0); // Паттерны должны быть уникальными
	terminals[current_node_index] = pattern_lengths.size();
	pattern_lengths.push_back(pattern.length());
}

std::set<Trie::Match> Trie::find_all(const std::string& text)
{
	assert(text.length() > 0);
	assert(pattern_lengths.size() > 0);
	std::set<Trie::Match> matches;
	int16_t current_node_index = 0;

	for (int i = 0; i < text.length(); ++i)
	{
		char symbol = text[i];
		current_node_index = transition(current_node_index, symbol);
		out(current_node_index, i, matches);
	}

	return matches;
}

int16_t Trie::get_suffix_link(int16_t state_index)
{
	if (suffix_links[state_index] == -1)
	{
		if (state_index == 0 || parents[state_index] == 0)
			suffix_links[state_index] = 0;
		else
			suffix_links[state_index] = transition(get_suffix_link(parents[state_index]), input_chars[state_index]);
	}
	return suffix_links[state_index];
}

int16_t Trie::get_terminal_suffix_link(int16_t state_index)
{
	if (terminal_links[state_index] == -1)
	{
		terminal_links[state_index] = get_suffix_link(state_index);
		while (terminal_links[state_index] != 0 &&
			terminals.count(terminal_links[state_index]) == 0)
			terminal_links[state_index] = get_suffix_link(terminal_links[state_index]);
	}
	return terminal_links[state_index];
}

int16_t Trie::transition(int16_t state_index, char symbol)
{
	std::pair<char, int16_t> key = std::make_pair(symbol, state_index);
	if (transitions.count(key) == 0)
	{
		if (state_index == 0)
			transitions[key] = 0;
		else
			transitions[key] = transition(get_suffix_link(state_index), symbol);
	}
	return transitions[key];
}

void Trie::out(int16_t state_index, size_t text_index, std::set<Trie::Match>& matches)
{
	if (terminals.count(state_index) == 0)
		state_index = get_terminal_suffix_link(state_index);
	while (state_index != 0)
	{
		size_t length = pattern_lengths[terminals[state_index]];
		Match match(text_index - length + 1, terminals[state_index]);
		matches.emplace(match);
		state_index = get_terminal_suffix_link(state_index);
	}
}

// Разбивает паттерн на подстроки, каждая из которых либо не содержит символов "?", либо состоит только из них
std::vector<std::string_view> split_pattern(const std::string& pattern)
{
	assert(pattern.length() > 0);
	std::vector<std::string_view> parts;
	size_t substr_start = 0;
	std::string_view pattern_view = std::string_view(pattern);
	bool is_prev_question = pattern[0] == '?';

	for (size_t i = 1; i < pattern.length(); ++i)
	{
		bool is_question = pattern[i] == '?';
		if (is_prev_question != is_question)
		{
			parts.emplace_back(pattern_view.substr(substr_start, i - substr_start));
			substr_start = i;
			is_prev_question = is_question;
		}
	}
	parts.emplace_back(pattern_view.substr(substr_start, pattern.length() - substr_start));
	return parts;
}

// Часть паттерна между символами "?"
struct SubPattern
{
	explicit SubPattern(int16_t index, int16_t length, int16_t questions_right)
		: index(index), length(length), questions_right(questions_right) {};

	int16_t index; // Индекс уникального субпаттерна в паттерне
	int16_t length; // Количество символов в субпаттерне
	int16_t questions_right; // Количество символов "?" справа от субпаттерна в паттерне
};

// Принимает на вход паттерн, возможно, содержащий символы "?"
// Возвращает:
// - subpatterns - вектор структур SubPattern, описывающих каждую часть патерна между символами "?"
// - unique_substrings - вектор уникальных частей патерна между символами "?" в порядке их первого появления в паттерне
// - leading_questions - количество ведущих нулей в паттерне
void parse_pattern(
	const std::string& pattern,
	std::vector<SubPattern>& subpatterns,
	std::vector<std::string_view>& unique_substrings,
	int16_t& leading_questions)
{
	std::unordered_map<std::string_view, int16_t> substrings_to_indices; // Сопоставляет каждому субпаттерну его номер
	std::vector<std::string_view> parts = split_pattern(pattern);

	size_t i = 0;
	std::string_view& part = parts[0];
	leading_questions = 0;

	if (part[0] == '?')
	{
		leading_questions = part.length();

		// Если паттерн состоит целиком из вопросов
		if (parts.size() == 1)
			return;
		++i;
	}

	while (i < parts.size())
	{
		part = parts[i];
		int16_t pattern_index = 0;

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

		++i; // Пытаемся обработать следующий блок вопросов, если он есть
		int16_t questions_right = 0;
		if (i < parts.size())
			questions_right = parts[i].length();
		SubPattern subpattern(pattern_index, part.length(), questions_right);
		subpatterns.emplace_back(subpattern);
		++i; // И переходим к следующему субпаттерну
	}
	return;
}

// Разбивает паттерн на субпаттерны (их описание помещается в subpatterns) и применяет алгоритм Ахо-Корасик
void find_partial_matches(
	const std::string& pattern,
	const std::string& text,
	std::vector<SubPattern>& subpatterns,
	std::set<Trie::Match>& matches,
	int16_t& leading_questions)
{
	std::vector<std::string_view> unique_substrings;
	parse_pattern(pattern, subpatterns, unique_substrings, leading_questions);

	// Если паттерн состоит только из знаков "?"
	if (subpatterns.size() == 0)
		return;

	// Создаём автомат Ахо-Корасик
	Trie trie;
	for (std::string_view& substr : unique_substrings)
		trie.add(substr);

	unique_substrings.clear();
	std::vector<std::string_view>().swap(unique_substrings);

	// Находим вхождения всех субпаттернов
	matches = trie.find_all(text);
}

// Возвращает индексы всех вхождений паттерна pattern (возможно, содержащего символы "?") в строку text
std::vector<size_t> find_all(const std::string& pattern, const std::string& text)
{
	assert(pattern.length() > 0);
	assert(text.length() > 0);
	std::vector<size_t> indices;

	if (pattern.length() > text.length())
		return indices;

	std::set<Trie::Match> matches;
	std::vector<SubPattern> parsed_pattern;
	int16_t leading_questions = 0;

	find_partial_matches(
		pattern,
		text,
		parsed_pattern,
		matches,
		leading_questions);

	// Если паттерн состоит только из знаков "?"
	if (parsed_pattern.size() == 0)
	{
		for (size_t i = 0; i <= text.length() - pattern.length(); ++i)
			indices.push_back(i);
		return indices;
	}

	if (matches.size() < parsed_pattern.size())
		return indices;

	// Теперь пытаемся сопоставить матчи с оригинальным паттерном (должен совпадать порядок + расстояние между ними)

	long skip_text_index = -1;

	for (Trie::Match match : matches)
	{
		size_t text_index = match.start;

		// Если данная позиция уже анализировалась, то пропускаем её
		if (text_index == skip_text_index)
			continue;

		skip_text_index = text_index;

		// Пропустим вхождения, слева от которых меньше символов, чем знаков "?" в начале паттерна
		if (text_index < leading_questions)
			continue;

		size_t match_must_start = text_index; // Позиция в тексте, в которой должен начинаться матч
		size_t matches_count = 0; // Количество совпавших субпаттернов
		for (SubPattern& subpattern : parsed_pattern)
		{
			// Если в данной позиции нет матча, соответствующего текущему субпаттерну, то нужно искать совпадение дальше
			if (matches.find(Trie::Match(match_must_start, subpattern.index)) == matches.end())
				break;

			// Расстояние между матчами должно совпадать с количеством вопросов между субпаттернами
			match_must_start += subpattern.length + subpattern.questions_right;

			// Случай, когда справа от конца текущего матча меньше символов, чем знаков "?" следом за ним в паттерне
			if (match_must_start > text.length())
				break;

			++matches_count;
		}
		if (matches_count != parsed_pattern.size())
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

	//pattern = "dabce?abc?bc";
	//text = "bcdabceaabcabc";

	std::vector<size_t> indices = find_all(pattern, text);
	for (size_t index : indices)
		std::cout << index << " ";

	std::cout.flush();
	return 0;
}

