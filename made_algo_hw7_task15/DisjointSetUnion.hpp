#pragma once

#include <vector>

// Система непересекающихся множеств
class DisjointSetUnion
{
public:
	explicit DisjointSetUnion(size_t size);

	size_t find_representative(size_t element); // Находит представителя множества, в котором находится element
	void merge(size_t left_representative, size_t right_representative); // Объединяет два множества, заданных их представителями

private:
	std::vector<size_t> parent; // Индекс родительского элемента в дереве
	std::vector<size_t> rank; // Оценка сверху высоты дерева
};