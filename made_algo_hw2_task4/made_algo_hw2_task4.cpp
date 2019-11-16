// Даны неотрицательные целые числа n, k и массив целых чисел из[0..10 ^ 9] размера n.
// Требуется найти k - ю порядковую статистику, т.е.напечатать число, которое бы стояло на позиции с индексом k(0..n - 1) в отсортированном массиве.
// Напишите нерекурсивный алгоритм.
// Требования к дополнительной памяти : O(n). Требуемое среднее время работы : O(n).
// 4_1.Реализуйте стратегию выбора опорного элемента “медиана трёх”.
// Функцию Partition реализуйте методом прохода двумя итераторами от начала массива к концу.

#include <iostream>
#include <assert.h>

// Возвращает индекс, в который помещается опорный элемент после разделения
// Предполагается, что изначально опорный элемент находится в конце массива
int partition(int* values, int n)
{	
	if (n <= 1) return 0;
	int pivot = values[n - 1];
	int i = 0; // указывает на начало группы элементов, строго бОльших опорного
	int j = 0; // указывает на первый нерассмотренный элемент
	while (j < n - 1)
	{
		if (values[j] > pivot)
			++j;
		else
			std::swap(values[i++], values[j++]);
	}
	std::swap(values[i], values[n - 1]);
	
	return i;
}

// Определяет опорный элемент по методу "медиана трёх" и меняет его с последним элементом
void set_pivot(int* values, int n)
{
	assert(n > 0);
	int first = 0;
	int last = n - 1;
	int mid = (first + last) / 2;
	// За две операции проталкиваем наибольший элемент вправо
	if (values[first] > values[mid])
		std::swap(values[first], values[mid]);
	if (values[mid] > values[last])
		std::swap(values[mid], values[last]);
	// Помещаем в начало меньший из двух элементов
	if (values[first] > values[mid])
		std::swap(values[first], values[mid]);
	
	// Медиана находится в позиции mid. Поменяем её с последним элементом
	std::swap(values[mid], values[last]);
}

// Возвращает k-ю порядковую статистику массива values
// Массив не изменяется
int get_k_order_statistic(const int* values, int n, int k)
{
	int* arr = new int[n];
	std::copy(values, values + n, arr);
	int* start = arr; // Начало подмассива, в котором будем искать k-ю порядковую статистику
	int size = n; // Размер этого подмассива
	int statistic = 0;
	while (true)
	{
		set_pivot(start, size); // Находим опроный элемент в подмассиве и помещаем его в конец
		int position = partition(start, size); // Позиция опорного элемента после разделения
		if (position == k)
		{
			statistic = *(start + position);
			break;
		}
		else if (position > k) // Искомая статистика находится левее опорного элемента
		{
			size = position;
		}
		else // Искомая статистика находится правее опорного элемента
		{
			start += position + 1;
			size -= position + 1;
			k -= position + 1;
		}
	}
	delete[] arr;
	return statistic;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	int n = 0;
	std::cin >> n;

	int k = 0;
	std::cin >> k;

	int* values = new int[n];

	for (int i = 0; i < n; ++i)
		std::cin >> values[i];

	int result = get_k_order_statistic(values, n, k);

	std::cout << result;
	std::cout.flush();
	delete[] values;
	return 0;
}
