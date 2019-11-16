// 3_2.Сортировка почти упорядоченной последовательности.
// Дана последовательность целых чисел a1...an и натуральное число k, такое что для любых i, j:
// если j >= i + k, то a[i] <= a[j]. Требуется отсортировать последовательность.
// Последовательность может быть очень длинной. Время работы O(n * log(k)). Память O(k). Использовать слияние.
//

#include <iostream>
#include <algorithm>

// Целочисленное деление с округлением вверх
int ceil_division(int x, int y)
{
	return (x + y - 1) / y;
}

// Сливает два отсортированных подмассива
void merge(int* first, int firstLen, int* second, int secondLen, int* result)
{
	int i = 0;
	int j = 0;
	int k = 0;
	while (i < firstLen && j < secondLen)
	{
		if (first[i] <= second[j])
			result[k++] = first[i++];
		else
			result[k++] = second[j++];
	}
	while (i < firstLen)
		result[k++] = first[i++];
	while (j < secondLen)
		result[k++] = second[j++];
}

// Последовательно движется слева направо, сортирует пары соседних подмассивов длины k и сливает их
void sort(int* values, int n, int k)
{
	int subarrays_count = ceil_division(n, k);
	for (int i = 0; i < subarrays_count - 1; ++i)
	{
		int* left_start = values + i * k;
		int* right_start = left_start + k;
		if (i == 0)
			std::sort(left_start, right_start);
		int right_len = std::min(k, n - (i + 1) * k);
		int* right_end = right_start + right_len;
		std::sort(right_start, right_end);
		int* merged = new int[k + right_len];
		merge(left_start, k, right_start, right_len, merged);
		for (int j = 0; j < k + right_len; ++j)
			left_start[j] = merged[j];
		delete[] merged;
	}
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

	sort(values, n, k);

	for (int i = 0; i < n; ++i)
		std::cout << values[i] << " ";

	std::cout.flush();
	delete[] values;
	return 0;
}

