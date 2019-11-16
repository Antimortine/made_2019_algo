// 3_2.Сортировка почти упорядоченной последовательности.
// Дана последовательность целых чисел a1...an и натуральное число k, такое что для любых i, j:
// если j >= i + k, то a[i] <= a[j]. Требуется отсортировать последовательность.
// Последовательность может быть очень длинной. Время работы O(n * log(k)). Память O(k). Использовать слияние.
//

#include <iostream>
#include <climits>

void sort(int* values, int n, int k)
{
	// Из условия следует, что в массиве содержится k отсортированных подмассивов,
	// каждый подмассив состоит из элементов, индексы которых имеют один и тот же остаток от деления на k.
	// Для использования слияния необходимо отслеживать текущую позицию ("голову") в каждом из подмассивов
	int* subarray_heads = new int[k];
	for (int i = 0; i < k; ++i)
		subarray_heads[i] = i;

	while (true)
	{
		// находим позицию минимальной головы
		int min_value = INT_MAX;
		int min_subarray = -1; // номер подмассива, голова которого минимальна
		int leftmost_head_position = INT_MAX; // расположение самой левой головы
		int leftmost_array_index = -1; // номер подмассива с самой левой головой
		for (int i = 0; i < k; ++i)
		{
			if (subarray_heads[i] >= n) continue;
			if (values[subarray_heads[i]] <= min_value)
			{
				min_value = values[subarray_heads[i]];
				min_subarray = i;
			}
			if (subarray_heads[i] <= leftmost_head_position)
			{
				leftmost_head_position = subarray_heads[i];
				leftmost_array_index = i;
			}
		}
		if (min_subarray == -1)
			break;
		std::swap(values[subarray_heads[min_subarray]], values[subarray_heads[leftmost_array_index]]);
		subarray_heads[leftmost_array_index] += k;
	}
	delete[] subarray_heads;
}

int main()
{
	int n = 0;
	std::cin >> n;
	
	int k = 0;
	std::cin >> k;

	int* values = new int[n];
	
	for (int i = 0; i < n; ++i)
		std::cin >> values[i];

	sort(values, n, k);

	for (int i = 0; i < n; ++i)
		std::cout << values[i] << std::endl;
	
	delete[] values;
	return 0;
}

