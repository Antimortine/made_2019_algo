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
#include <map>
#include <unordered_map>
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
		std::map<char, Node*> next; // Узлы, в которые можно непосредственно попасть из текущего, прочитав подходящий символ
		std::map<char, Node*> transition; // Узлы, в которые можно попасть либо напрямую, либо по суффиксным ссылкам
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
		if (current->next.find(symbol) == current->next.end())
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
	if (state->transition.find(symbol) == state->transition.end())
	{
		if (state->next.find(symbol) != state->next.end())
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
			parts.push_back(pattern.substr(substr_start, i - substr_start));
			substr_start = i;
			is_prev_question = is_question;
		}
	}
	parts.push_back(pattern.substr(substr_start, pattern.length() - substr_start));
	return parts;
}

// Часть паттерна между символами "?"
struct SubPattern
{
	int index; // Индекс уникального субпаттерна в паттерне
	size_t length; // Количество символов в субпаттерне
	size_t questions_left = 0; // Количество символов "?" слева от субпаттерна в паттерне
	size_t questions_right = 0; // Количество символов "?" справа от субпаттерна в паттерне

};

// Принимает на вход паттерн, возможно, содержащий символы "?"
// Возвращает пару:
// - первый элемент - вектор структур SubPattern, описывающих каждую часть патерна между символами "?"
// - второй элемент - вектор уникальных частей патерна между символами "?" в порядке их первого появления в паттерне
std::pair<std::vector<SubPattern>, std::vector<std::string>> parse_pattern(const std::string& pattern)
{
	std::map<std::string, size_t> substrings_to_indices; // Сопоставляет каждому субпаттерну его номер
	std::vector<SubPattern> subpatterns; // Вектор структур SubPattern, описывающих каждую часть патерна между символами "?"
	std::vector<std::string> unique_substrings; // Вектор уникальных частей патерна между символами "?" в порядке их первого появления в паттерне

	std::vector<std::string> parts = split_pattern(pattern);

	size_t questions_left = 0;
	size_t i = 0;
	std::string& part = parts[i];

	if (part[0] == '?')
	{
		questions_left = part.length();
		// Если вдруг паттерн состоит целиком из вопросов
		if (parts.size() == 1)
		{
			subpatterns.push_back(SubPattern{ -1, 0, questions_left, 0 });
			return std::make_pair(subpatterns, unique_substrings);
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
			unique_substrings.push_back(part);
		}
		else
		{
			pattern_index = substrings_to_indices[part];
		}
		SubPattern subpattern{ static_cast<int>(pattern_index), part.length(), questions_left };
		++i; // Пытаемся обработать следующий блок вопросов, если он есть
		if (i < parts.size())
		{
			questions_left = parts[i].length();
			subpattern.questions_right = questions_left;
		}
		subpatterns.push_back(subpattern);
		++i; // И переходим к следующему паттерну
	}
	return std::make_pair(subpatterns, unique_substrings);
}

// Возвращает индексы всех вхождений паттерна pattern (возможно, содержащего символы "?") в строку text
std::vector<size_t> find_all(const std::string& pattern, const std::string& text)
{
	assert(pattern.length() > 0);
	assert(text.length() > 0);
	std::vector<size_t> indices;

	if (pattern.length() > text.length())
		return indices;

	auto parsed = parse_pattern(pattern);
	std::vector<SubPattern> subpatterns = parsed.first;
	std::vector<std::string> unique_substrings = parsed.second;

	// Если паттерн состоит только из знаков "?"
	if (subpatterns[0].index == -1)
	{
		for (size_t i = 0; i <= text.length() - pattern.length(); ++i)
			indices.push_back(i);
		return indices;
	}

	// Создаём автомат Ахо-Корасик
	Trie* trie = new Trie();
	for (std::string& substr : unique_substrings)
		trie->add(substr);

	// Находим вхождения всех субпаттернов
	std::vector<Trie::Match> matches = trie->find_all(text);

	delete trie;
	unique_substrings.clear();
	std::vector<std::string>().swap(unique_substrings);

	if (matches.size() < subpatterns.size())
		return indices;

	// Каждый элемент map хранит все матчи, начинающиеся в одной и той же позиции текста
	// Ключ в unordered_map - индекс (идентификатор) уникального субпаттерна в оригинальном шаблоне
	std::map<size_t,std::unordered_map<int, Trie::Match>> matches_by_start;
	for (Trie::Match& match : matches)
	{
		SubPattern& subpattern = subpatterns[match.pattern_index];
		// Пропустим матчи, слева от которых меньше символов, чем знаков "?" в начале паттерна
		if (match.start < subpattern.questions_left)
			continue;
		// Пропустим матчи, справа от которых меньше символов, чем знаков "?" в конце паттерна
		if (match.start + match.length > text.length() - subpattern.questions_right)
			continue;
		if (matches_by_start.find(match.start) == matches_by_start.end())
		{
			std::unordered_map<int, Trie::Match> matches_for_current_start;
			matches_for_current_start.emplace(match.pattern_index, match);
			matches_by_start.emplace(match.start, matches_for_current_start);
		}
		else
		{
			std::unordered_map<int, Trie::Match>& matches_for_current_start = matches_by_start.at(match.start);
			matches_for_current_start.emplace(match.pattern_index, match);
		}
	}
	matches.clear();
	std::vector<Trie::Match>().swap(matches);

	// Теперь пытаемся сопоставить матчи с оригинальным паттерном (должен совпадать порядок + расстояние между ними)
	size_t leading_questions = subpatterns[0].questions_left; // Количество начальных символов "?" в паттерне

	for (std::pair<size_t, std::unordered_map<int, Trie::Match>> pair : matches_by_start)
	{
		size_t text_index = pair.first;
		size_t match_must_start = text_index; // Позиция в тексте, в которой должен начинаться матч
		size_t matches_count = 0;
		for (SubPattern& subpattern : subpatterns)
		{
			if (matches_by_start.find(match_must_start) == matches_by_start.end())
				break;
			std::unordered_map<int, Trie::Match>& matches_for_current_start = matches_by_start.at(match_must_start);

			// Если в данной позиции нет матча, соответствующего текущему субпаттерну, то нужно искать совпадение дальше
			if (matches_for_current_start.count(subpattern.index) == 0)
				break;

			Trie::Match& match = matches_for_current_start.at(subpattern.index);

			// Расстояние между матчами должно совпадать с количеством вопросов между субпаттернами
			match_must_start = match.start + match.length + subpattern.questions_right;
			++matches_count;
		}
		if (matches_count != subpatterns.size())
			continue;

		// Ура, нашли вхождение всего паттерна
		indices.push_back(text_index - leading_questions);
	}

	return indices;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string pattern;
	std::string text;

	//std::cin >> pattern >> text;

	pattern = "a?aa?aaa??a?aa?";
	text = "aaaaaaaaaaaaaaa";

	std::vector<size_t> indices = find_all(pattern, text);
	for (size_t index : indices)
		std::cout << index << " ";

	std::cout.flush();
	return 0;
}
