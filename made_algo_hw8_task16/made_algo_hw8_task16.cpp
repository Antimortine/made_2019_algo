// Задача 16. Поиск подстроки (5 баллов)
// Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки ­– n. Время O(n + p), доп.память – O(p).
// Вариант 1. С помощью префикс-функции (алгоритм Кнута - Морриса - Пратта).

#include <iostream>
#include <string>
#include <vector>

// Возвращает вектор значений префикс-функции для строки str
std::vector<size_t> build_prefix_function(const std::string& str)
{
	size_t str_len = str.length();
	std::vector<size_t> pi(str_len, 0); // Значения префикс-функции

	for (size_t i = 1; i < str_len; ++i)
	{
		// Пытаемся определить наибольшую длину несобственного суффикса, являющегося префиксом, оканчивающегося в позиции i
		// Для начала возьмём суффикс с теми же свойствами, но оканчивающийся в позиции i-1
		// Если его можно дополнить символом str[i] и снова получить префикс str, то достаточно положить pi[i] = pi[i - 1] + 1
		// Иначе пытаемся найти более короткий (возможно, пустой) суффикс с теми же свойствами
		size_t prefix_len = pi[i - 1];
		while (prefix_len > 0 && str[prefix_len] != str[i])
			prefix_len = pi[prefix_len - 1];

		if (str[prefix_len] == str[i])
			++prefix_len;

		pi[i] = prefix_len;
	}

	return pi;
}

// Возвращает индексы всех вхождений образца pattern в строку text
// Реализует алгоритм Кнута - Морриса - Пратта
std::vector<size_t> kmp_find_all(const std::string& pattern, const std::string& text)
{
	std::vector<size_t> indices; // Индексы вхождений образца в текст
	std::vector<size_t> pi = build_prefix_function(pattern);
	size_t pattern_len = pattern.length();
	size_t text_len = text.length();

	size_t match_len = 0; // количество символов, совпадающих с префиксом шаблона
	for (size_t i = 0; i < text_len; ++i)
	{
		// определяем максимальную длину префикса шаблона, который можно дополнить символом text[i]
		while (match_len > 0 && pattern[match_len] != text[i])
			match_len = pi[match_len - 1];

		if (pattern[match_len] == text[i])
			++match_len;

		if (match_len == pattern_len)
		{
			indices.push_back(i - pattern_len + 1);
			match_len = pi[match_len - 1]; // ищем вхождения шаблона с перекрытиями
		}
	}

	return indices;
}


int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string pattern;
	std::string text;

	std::getline(std::cin, pattern);
	std::getline(std::cin, text);

	std::vector<size_t> pattern_indices = kmp_find_all(pattern, text);

	for (size_t index : pattern_indices)
	{
		std::cout << index << " ";
	}

	std::cout.flush();
	return 0;
}
