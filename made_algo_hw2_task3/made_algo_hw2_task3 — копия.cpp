// 3_2.���������� ����� ������������� ������������������.
// ���� ������������������ ����� ����� a1...an � ����������� ����� k, ����� ��� ��� ����� i, j:
// ���� j >= i + k, �� a[i] <= a[j]. ��������� ������������� ������������������.
// ������������������ ����� ���� ����� �������. ����� ������ O(n * log(k)). ������ O(k). ������������ �������.
//

#include <iostream>
#include <climits>

void sort(int* values, int n, int k)
{
	// �� ������� �������, ��� � ������� ���������� k ��������������� �����������,
	// ������ ��������� ������� �� ���������, ������� ������� ����� ���� � ��� �� ������� �� ������� �� k.
	// ��� ������������� ������� ���������� ����������� ������� ������� ("������") � ������ �� �����������
	int* subarray_heads = new int[k];
	for (int i = 0; i < k; ++i)
		subarray_heads[i] = i;

	while (true)
	{
		// ������� ������� ����������� ������
		int min_value = INT_MAX;
		int min_subarray = -1; // ����� ����������, ������ �������� ����������
		int leftmost_head_position = INT_MAX; // ������������ ����� ����� ������
		int leftmost_array_index = -1; // ����� ���������� � ����� ����� �������
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

