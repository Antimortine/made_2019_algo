﻿// Задача 17. Шаблон с ? (5 баллов)
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

// Часть паттерна между символами "?"
struct SubPattern
{
	explicit SubPattern(size_t length, size_t questions_right)
		: length(length), questions_right(questions_right) {};

	size_t length; // Количество символов в субпаттерне
	size_t questions_right; // Количество символов "?" справа от субпаттерна в паттерне
};

class PatternParser
{
public:
	// Разбивает паттерн на подстроки, каждая из которых либо не содержит символов "?", либо состоит только из них
	static std::vector<std::string_view> split_pattern(const std::string& pattern);

	// Принимает на вход паттерн, возможно, содержащий символы "?"
	// Возвращает:
	// - subpatterns - вектор структур SubPattern, описывающих каждую часть патерна между символами "?"
	// - unique_substrings - вектор уникальных частей патерна между символами "?" в порядке их первого появления в паттерне
	// - leading_questions - количество ведущих нулей в паттерне
	static void parse_pattern(
		const std::string& pattern,
		std::vector<SubPattern>& subpatterns,
		std::vector<std::string_view>& unique_substrings,
		size_t& leading_questions);
};

std::vector<std::string_view> PatternParser::split_pattern(const std::string& pattern)
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

void PatternParser::parse_pattern(
	const std::string& pattern,
	std::vector<SubPattern>& subpatterns,
	std::vector<std::string_view>& substrings,
	size_t& leading_questions)
{
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
		substrings.emplace_back(part);

		++i; // Пытаемся обработать следующий блок вопросов, если он есть
		size_t questions_right = 0;
		if (i < parts.size())
			questions_right = parts[i].length();

		subpatterns.emplace_back(SubPattern(part.length(), questions_right));
		++i; // И переходим к следующему субпаттерну
	}
}

namespace std
{
	template <>
	struct hash<std::pair<char, size_t>>
	{
		size_t operator()(const std::pair<char, size_t>& key) const noexcept
		{
			return 37 * static_cast<size_t>(key.first) + key.second;
		}
	};
}

// Позволяет находить все вхождения паттерна (возможно, содержащего символ "?") в строку с помощью алгоритма Ахо-Корасик
class PatternMatcher
{
public:
	struct Node
	{
		explicit Node(size_t parent, char input_char)
			: parent(parent), input_char(input_char) {};
		size_t parent; // Индекс родительского узла
		char input_char; // Символ, по которому в данный узел можно попасть из родительского
		long suffix_link = -1; // Суффиксная ссылка
		long terminal_link = -1; // Суффиксная ссылка в ближайшую терминальную вершину
		std::vector<size_t> terminals; // Индексы (в векторе subpatterns) субпаттернов, которые заканчиваются  в данном узле

		// Функция перехода
		// Сопаставляет символу индекс узла, в который можно попасть либо напрямую, либо по суффиксным ссылкам
		std::unordered_map<char, size_t> transitions;
	};

	PatternMatcher(const std::string& pattern);
	PatternMatcher(const PatternMatcher&) = delete;
	PatternMatcher(PatternMatcher&&) = delete;
	PatternMatcher& operator=(const PatternMatcher&) = delete;
	PatternMatcher& operator=(PatternMatcher&&) = delete;
	~PatternMatcher() = default;

	// Возвращает индексы всех вхождений паттерна в строку text
	std::vector<size_t> find_all(const std::string& text);

private:
	size_t subpatterns_added = 0;
	size_t pattern_length = 0;
	size_t leading_questions = 0; // Количество ведущих знаков "?" в паттерне
	std::vector<SubPattern> subpatterns; // Оригинальный паттерн в виде вектора субпаттернов
	std::vector<Node> nodes;

	// Если ключ (позиция_в_тексте, индекс_субпаттерна) находится в chain_starts, то это допустимое продолжение
	// Значение в этом случае хранит позицию вхождения первого субпаттерна
	// Любое вхождение первого субпаттерна допустимо и здесь не учитывается
	std::map<std::pair<size_t, size_t>, size_t> chain_starts;

	void add_subpattern(const std::string_view& subpattern); // Добавляет субпаттерн в бор
	size_t get_suffix_link(size_t state_index); // Возвращает суффиксную ссылку
	size_t get_terminal_suffix_link(size_t state_index); // Возвращает терминальную суффиксную ссылку
	size_t transition(size_t state_index, char symbol); // Функция переходов автомата из состояния state_index по ребру symbol

	// Функция выхода: добавляет в matches индекс вхождения всего паттерна для вершины nodes[state_index]
	// и позиции в тексте text_index
	void out(size_t state_index, size_t text_index, size_t text_length, std::vector<size_t>& matches);
};

PatternMatcher::PatternMatcher(const std::string& pattern)
{
	pattern_length = pattern.length();

	std::vector<std::string_view> substrings;
	PatternParser::parse_pattern(pattern, subpatterns, substrings, leading_questions);

	size_t subpatterns_total_length = 0;
	for (std::string_view& substr : substrings)
		subpatterns_total_length += substr.length();

	nodes.reserve(subpatterns_total_length + 1);
	Node root(0, 0);
	root.suffix_link = root.terminal_link = 0;
	nodes.emplace_back(root);

	for (std::string_view& substr : substrings)
		add_subpattern(substr);
}

void PatternMatcher::add_subpattern(const std::string_view& subpattern)
{
	assert(subpattern.length() > 0);
	size_t current_node_index = 0;
	for (char symbol : subpattern)
	{
		Node& current_node = nodes[current_node_index];

		// Если такого перехода нет, то создадим его вместе с новой вершиной
		if (current_node.transitions.count(symbol) == 0)
		{
			size_t next_node_index = nodes.size();
			Node next_node(current_node_index, symbol);
			current_node.transitions[symbol] = next_node_index;
			nodes.emplace_back(next_node);
			current_node_index = next_node_index;
		}
		else
			current_node_index = current_node.transitions[symbol];
	}
	nodes[current_node_index].terminals.push_back(subpatterns_added++);
}

std::vector<size_t> PatternMatcher::find_all(const std::string& text)
{
	size_t text_length = text.length();
	assert(text_length > 0);
	std::vector<size_t> matches;

	if (pattern_length > text_length)
		return matches;

	// Если паттерн состоит только из знаков "?"
	if (subpatterns.size() == 0)
	{
		for (size_t i = 0; i <= text_length - leading_questions; ++i)
			matches.push_back(i);
		return matches;
	}

	size_t trailing_questions = subpatterns.back().questions_right;
	size_t current_node_index = 0;
	for (size_t i = leading_questions; i < text_length - trailing_questions; ++i)
	{
		char symbol = text[i];
		current_node_index = transition(current_node_index, symbol);
		out(current_node_index, i, text_length, matches);
	}

	return matches;
}

size_t PatternMatcher::get_suffix_link(size_t state_index)
{
	Node& node = nodes[state_index];
	if (node.suffix_link == -1)
	{
		if (node.parent == 0)
			node.suffix_link = 0;
		else
			node.suffix_link = transition(get_suffix_link(node.parent), node.input_char);
	}
	return static_cast<size_t>(node.suffix_link);
}

size_t PatternMatcher::get_terminal_suffix_link(size_t state_index)
{
	Node& node = nodes[state_index];
	if (node.terminal_link == -1)
	{
		node.terminal_link = get_suffix_link(state_index);
		while (node.terminal_link != 0 && nodes[node.terminal_link].terminals.size() == 0)
			node.terminal_link = get_suffix_link(node.terminal_link);
	}
	return static_cast<size_t>(node.terminal_link);
}

size_t PatternMatcher::transition(size_t state_index, char symbol)
{
	Node& node = nodes[state_index];
	if (node.transitions.count(symbol) == 0)
	{
		if (state_index == 0)
			node.transitions[symbol] = 0;
		else
			node.transitions[symbol] = transition(get_suffix_link(state_index), symbol);
	}
	return node.transitions[symbol];
}

void PatternMatcher::out(size_t state_index, size_t text_index, size_t text_length, std::vector<size_t>& matches)
{
	// Удаляем устаревшие записи в chain_starts
	if (chain_starts.size() > 0 && chain_starts.begin()->first.first < text_index)
	{
		auto end_iter = chain_starts.lower_bound(std::make_pair(text_index, 0));
		chain_starts.erase(chain_starts.begin(), end_iter);
	}

	if (nodes[state_index].terminals.size() == 0)
		state_index = get_terminal_suffix_link(state_index);
	while (state_index != 0)
	{
		Node& node = nodes[state_index];

		// Если это единственный субпаттерн, то найдено вхождение исходного паттерна
		if (subpatterns.size() == 1)
		{
			size_t subpattern_start = text_index - subpatterns[0].length + 1;
			matches.push_back(subpattern_start - leading_questions);
			return;
		}
		// Иначе перебираем все субпаттерны, которые оканчиваются в данной вершине
		for (size_t subpattern_index : node.terminals)
		{
			std::pair<size_t, size_t> current_key = std::make_pair(text_index, subpattern_index);

			// Если это последний субпаттерн, то надо проверить, завершает ли он цепочку
			if (subpattern_index == subpatterns.size() - 1)
			{
				if (chain_starts.find(current_key) != chain_starts.end())
					matches.push_back(chain_starts[current_key]);
				continue;
			}

			SubPattern& current_subpattern = subpatterns[subpattern_index];
			SubPattern& next_subpattern = subpatterns[subpattern_index + 1];
			size_t next_subpattern_end = text_index + current_subpattern.questions_right + next_subpattern.length;
			if (next_subpattern_end >= text_length)
				continue;

			std::pair<size_t, size_t> next_key = std::make_pair(next_subpattern_end, subpattern_index + 1);

			// Если это первый субпаттерн паттерна, его в любом случае нужно обработать:
			// сохранить позицию вхождения в текст + добавить информацию о том, что можно искать следующий субпаттерн
			if (subpattern_index == 0)
			{
				size_t subpattern_length = current_subpattern.length;
				size_t subpattern_start = text_index - subpattern_length + 1;
				chain_starts.emplace(std::make_pair(next_key, subpattern_start - leading_questions));
			}
			// Иначе нужно убедиться, что это вхождение продолжает цепочку
			else if (chain_starts.find(current_key) != chain_starts.end())
			{
				chain_starts.emplace(std::make_pair(next_key, chain_starts[current_key]));
			}
		}
		// И продолжаем искать другие совпадения субпаттернов
		state_index = get_terminal_suffix_link(state_index);
	}
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string pattern;
	std::string text;

	std::cin >> pattern >> text;

	//pattern = "??";
	//text = "bbaaaaaaaaaaaaa";
	// 0 1 2 3 4 5 6 7 8 9 10 11 12 13

	//pattern = "a??";
	//text = "abcaaaaab";
	// 0 3 4 5 6 

	//pattern = "a?aa?aaa??a?aa?";
	//text = "aaaaaaaaaaaaaaa";
	// 0

	//pattern = "ab??aba";
	//text = "ababacaba";
	// 2

	//pattern = "aa??bab?cbaa?";
	//text = "aabbbabbcbaabaabbbabbcbaab";
	// 0 13

	//pattern = "?ab??aba";
	//text = "ababacaba";
	// 1

	//pattern = "ba?aab?abab";
	//text = "aababab";
	// -

	//pattern = "ba?aab?abab";
	//text = "ababaabaababb";
	// 1

	PatternMatcher matcher(pattern);

	std::vector<size_t> indices = matcher.find_all(text);
	for (size_t index : indices)
		std::cout << index << " ";

	std::cout.flush();
	return 0;
}

