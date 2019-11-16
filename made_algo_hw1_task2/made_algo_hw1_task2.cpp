// 2_2.Быстрое сложение.
// Для сложения чисел используется старый компьютер.Время, затрачиваемое на нахождение суммы двух чисел равно их сумме.
// Таким образом для нахождения суммы чисел 1, 2, 3 может потребоваться разное время, в зависимости от порядка вычислений.
// ((1 + 2) + 3) -> 1 + 2 + 3 + 3 = 9
// ((1 + 3) + 2) -> 1 + 3 + 4 + 2 = 10
// ((2 + 3) + 1) -> 2 + 3 + 5 + 1 = 11
// Требуется написать программу, которая определяет минимальное время, достаточное для вычисления суммы заданного набора чисел.
//

#include <assert.h>
#include <iostream>
#include <vector>

// Куча, в которой каждый узел не превосходит её потомков
class MinHeap
{
public:
	// Проверка на пустоту
	bool empty() const;
	// Добавление элемента
	void push(int value);
	// Извлечение минимального элемента
	int pop();
	// Количество элементов
	int size() const;

private:
	std::vector<int> array;

	// поднимает элемент, который меньше родительского
	void sift_up(int i);
	// спускает элемент, который больше дочерних
	void sift_down(int i);
};

bool MinHeap::empty() const
{
	return array.empty();
}

void MinHeap::push(int value)
{
	array.push_back(value);
	sift_up(size() - 1);
}

int MinHeap::pop()
{
	assert(!empty());
	int result = array[0];
	array[0] = array.back();
	array.pop_back();
	if (size() > 0)
		sift_down(0);
	return result;
}

int MinHeap::size() const
{
	return array.size();
}

void MinHeap::sift_up(int i)
{
	assert(i >= 0 && i < size());
	if (i == 0) return;
	int parent_index = (i - 1) / 2;
	if (array[i] < array[parent_index])
	{
		std::swap(array[i], array[parent_index]);
		sift_up(parent_index);
	}
}

void MinHeap::sift_down(int i)
{
	assert(i >= 0 && i < size());
	if (i >= size() / 2 || size() == 1) return; // Листовые узлы просеивать некуда
	int left_child_index = 2 * i + 1; // Благодаря проверке выше существует всегда
	int right_child_index = 2 * i + 2; // Может отсутствовать!
	bool has_right_child = right_child_index != size();
	// Если узел не превосходит своих потомков, то просеивать ничего не нужно
	if (array[i] <= array[left_child_index] &&
		(!has_right_child || array[i] <= array[right_child_index]))
		return;
	// Иначе меняем узел с наименьшим из потомков
	if (!has_right_child || array[left_child_index] <= array[right_child_index])
	{
		std::swap(array[i], array[left_child_index]);
		sift_down(left_child_index);
	}
	else
	{
		std::swap(array[i], array[right_child_index]);
		sift_down(right_child_index);
	}
}

// Возвращает минимальное количество операций, необходимое для сложения чисел в куче
int get_min_operations(MinHeap& heap)
{
	int operations = 0;
	while (heap.size() > 1)
	{
		int first_number = heap.pop();
		int second_number = heap.pop();
		int sum = first_number + second_number;
		operations += sum;
		heap.push(sum);
	}
	return operations;
}

int main()
{
	int n = 0;
	std::cin >> n;

	MinHeap heap;

	for (int i = 0; i < n; ++i)
	{
		int value = 0;
		std::cin >> value;
		heap.push(value);
	}
	std::cout << get_min_operations(heap);
	return 0;
}
