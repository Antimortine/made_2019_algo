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

// Часть паттерна между символами "?"
struct SubPattern
{
	explicit SubPattern(int16_t id, int16_t questions_right)
		: id(id), questions_right(questions_right) {};

	int16_t id; // Индекс уникального субпаттерна в паттерне
	int16_t questions_right; // Количество символов "?" справа от субпаттерна в паттерне
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
		int16_t& leading_questions);
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
		SubPattern subpattern(pattern_index, questions_right);
		subpatterns.emplace_back(subpattern);
		++i; // И переходим к следующему субпаттерну
	}
	return;
}

namespace std
{
	template <>
	struct hash<std::pair<char, int16_t>>
	{
		size_t operator()(const std::pair<char, int16_t>& key) const
		{
			return 37 * static_cast<size_t>(key.first) + static_cast<size_t>(key.second);
		}
	};

	template <>
	struct hash<std::pair<size_t, int16_t>>
	{
		size_t operator()(const std::pair<size_t, int16_t>& key) const
		{
			return 37 * static_cast<size_t>(key.first) + static_cast<size_t>(key.second);
		}
	};
}

// Позволяет находить все вхождения паттерна (возможно, содержащего символ "?") в строку с помощью алгоритма Ахо-Корасик
class PatternMatcher
{
public:
	PatternMatcher(const std::string& pattern);
	PatternMatcher(const PatternMatcher&) = delete;
	PatternMatcher(PatternMatcher&&) = delete;
	PatternMatcher& operator=(const PatternMatcher&) = delete;
	PatternMatcher& operator=(PatternMatcher&&) = delete;
	~PatternMatcher() = default;

	// Возвращает индексы всех вхождений паттерна в строку text
	std::vector<size_t> find_all(const std::string& text);

private:
	int16_t nodes_count = 1;
	int16_t pattern_length = 0;
	int16_t leading_questions = 0; // Количество ведущих знаков "?" в паттерне
	std::vector<SubPattern> subpatterns; // Оригинальный паттерн в виде вектора субпаттернов
	std::vector<int16_t> subpattern_lengths; // Длины субпаттернов (для каждого уникального субпаттерна)
	std::vector<std::vector<int16_t>> subpattern_ids_to_indices; // Для каждого уникального субпаттерна хранит его позиции в исходном паттерне

	std::vector<int16_t> parents; // Индексы родительских узлов
	std::vector<char> input_chars; // Для каждого узла хранит символ, по которому в него можно попасть из родительского
	std::vector<int16_t> suffix_links; // Суффиксные ссылки
	std::vector<int16_t> terminal_links; // Суффиксные ссылки в ближайшую терминальную вершину

	// Функция перехода
	// Паре (индекс узла, символ) сопаставляет индекс узла, в который можно попасть либо напрямую, либо по суффиксным ссылкам
	std::unordered_map<std::pair<char, int16_t>, int16_t> transitions;

	// Сопоставляет номерам узлов индексы субпаттернов, которые там заканчиваются
	std::map<int16_t, int16_t> terminals;

	// Если ключ (позиция_в_тексте, индекс_субпаттерна) находится в chain_starts, то это допустимое продолжение
	// Значение в этом случае хранит позицию вхождения первого субпаттерна
	// Любое вхождение первого субпаттерна допустимо и здесь не учитывается
	std::map<std::pair<size_t, int16_t>, size_t> chain_starts;

	void add_subpattern(const std::string_view& subpattern); // Добавляет субпаттерн в бор
	int16_t get_suffix_link(int16_t state_index); // Возвращает суффиксную ссылку
	int16_t get_terminal_suffix_link(int16_t state_index); // Возвращает терминальную суффиксную ссылку
	int16_t transition(int16_t state_index, char symbol); // Функция переходов автомата из состояния state_index по ребру symbol

	// Функция выхода: добавляет в matches индекс вхождения всего паттерна для вершины nodes[state_index]
	// и позиции в тексте text_index
	void out(int16_t state_index, size_t text_index, std::vector<size_t>& matches);
};

PatternMatcher::PatternMatcher(const std::string& pattern)
{
	pattern_length = pattern.length();

	std::vector<std::string_view> unique_substrings;
	PatternParser::parse_pattern(pattern, subpatterns, unique_substrings, leading_questions);

	size_t subpatterns_total_length = 0;
	for (std::string_view& substr : unique_substrings)
		subpatterns_total_length += substr.length();

	parents.reserve(subpatterns_total_length);
	input_chars.reserve(subpatterns_total_length);
	suffix_links.reserve(subpatterns_total_length);
	terminal_links.reserve(subpatterns_total_length);
	transitions.reserve(subpatterns_total_length);

	parents.push_back(0);
	input_chars.push_back(0);
	suffix_links.push_back(0);
	terminal_links.push_back(0);

	for (std::string_view& substr : unique_substrings)
		add_subpattern(substr);

	subpattern_ids_to_indices = std::vector<std::vector<int16_t>>(unique_substrings.size(), std::vector<int16_t>());
	for (size_t i = 0; i < subpatterns.size(); ++i)
		subpattern_ids_to_indices[subpatterns[i].id].push_back(i);
}

void PatternMatcher::add_subpattern(const std::string_view& subpattern)
{
	assert(subpattern.length() > 0);
	int current_node_index = 0;
	for (char symbol : subpattern)
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
	terminals[current_node_index] = subpattern_lengths.size();
	subpattern_lengths.push_back(subpattern.length());
}

std::vector<size_t> PatternMatcher::find_all(const std::string& text)
{
	assert(text.length() > 0);
	std::vector<size_t> matches;

	if (pattern_length > text.length())
		return matches;

	// Если паттерн состоит только из знаков "?"
	if (subpatterns.size() == 0)
	{
		for (size_t i = 0; i <= text.length() - leading_questions; ++i)
			matches.push_back(i);
		return matches;
	}

	int16_t trailing_questions = subpatterns.back().questions_right;
	int16_t current_node_index = 0;
	for (int i = leading_questions; i < text.length() - trailing_questions; ++i)
	{
		char symbol = text[i];
		current_node_index = transition(current_node_index, symbol);
		out(current_node_index, i, matches);
	}

	return matches;
}

int16_t PatternMatcher::get_suffix_link(int16_t state_index)
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

int16_t PatternMatcher::get_terminal_suffix_link(int16_t state_index)
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

int16_t PatternMatcher::transition(int16_t state_index, char symbol)
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

void PatternMatcher::out(int16_t state_index, size_t text_index, std::vector<size_t>& matches)
{
	// Удаляем устаревшие записи в chain_starts
	if (chain_starts.size() > subpatterns.size() * 2)
	{
		auto end_iter = chain_starts.upper_bound(std::make_pair(text_index, -1));
		chain_starts.erase(chain_starts.begin(), end_iter);
	}

	if (terminals.find(state_index) == terminals.end())
		state_index = get_terminal_suffix_link(state_index);
	while (state_index != 0)
	{
		// Если это единственный субпаттерн, то найдено вхождение исходного паттерна
		if (subpatterns.size() == 1)
		{
			size_t subpattern_start = text_index - subpattern_lengths[0] + 1;
			matches.push_back(subpattern_start - leading_questions);
			return;
		}

		// Идентификатор уникального субпаттерна, вхождение которого нашли
		int16_t subpattern_id = terminals[state_index];

		// Позиции этого субпаттерна в оригинальном паттерне
		std::vector<int16_t>& subpattern_indices = subpattern_ids_to_indices[subpattern_id];
		for (int16_t subpattern_index : subpattern_indices)
		{
			std::pair<size_t, int16_t> current_key = std::make_pair(text_index, subpattern_index);

			// Если это последний субпаттерн, то надо проверить, завершает ли он цепочку
			if (subpattern_index == subpatterns.size() - 1)
			{
				if (chain_starts.find(current_key) != chain_starts.end())
					matches.push_back(chain_starts[current_key]);
				continue;
			}

			SubPattern& current_subpattern = subpatterns[subpattern_index];
			SubPattern& next_subpattern = subpatterns[subpattern_index + 1];
			size_t next_subpattern_end = text_index + current_subpattern.questions_right + subpattern_lengths[next_subpattern.id];
			std::pair<size_t, int16_t> next_key = std::make_pair(next_subpattern_end, subpattern_index + 1);

			// Если это первый субпаттерн паттерна, его в любом случае нужно обработать:
			// сохранить позицию вхождения в текст + добавить информацию о том, что можно искать следующий субпаттерн
			if (subpattern_index == 0)
			{
				size_t subpattern_length = subpattern_lengths[subpattern_id];
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

	//pattern = "ba?aab?abab";
	//text = "ababaabaababb";

	PatternMatcher matcher(pattern);

	std::vector<size_t> indices = matcher.find_all(text);
	for (size_t index : indices)
		std::cout << index << " ";

	std::cout.flush();
	return 0;
}

