//Задача 15 (5 баллов)
//«Приближенное решение метрической неориентированной задачи коммивояжера».Найдите приближенное решение метрической неориентированной задачи коммивояжера в полном графе(на плоскости) с помощью минимального остовного дерева.
//Оцените качество приближения на случайном наборе точек, нормально распределенном на плоскости с дисперсией 1. Нормально распределенный набор точек получайте с помощью преобразования Бокса - Мюллера.
//При фиксированном N, количестве вершин графа, несколько раз запустите оценку качества приближения.Вычислите среднее значение и среднеквадратичное отклонение качества приближения для данного N.
//Запустите данный эксперимент для всех N в некотором диапазоне, например, [2, 10].
//Автоматизируйте запуск экспериментов.
//В решении требуется разумно разделить код на файлы.Каждому классу - свой заголовочный файл и файл с реализацией.

#include "ExperimentsRunner.hpp"
#include <iostream>
#include <fstream>

int main()
{
	ExperimentsRunner experiments_runner;

	std::ofstream output("experiment_results.tsv");
	output << "graph_size\tn_trials\tratio_mean\tRatio_stddev\n";

	size_t min_size = 2;
	size_t max_size = 10;
	size_t trials = 30;

	for (size_t i = min_size; i <= max_size; ++i)
	{
		std::cout << "Running experiment #" << i - min_size + 1 << " out of " << max_size - min_size + 1 << "\n";
		auto result = experiments_runner.run_experiment(i, trials);
		output << result.graph_size << "\t" << result.trials << "\t" << result.approximation_ratio_mean << "\t" << result.approximation_ratio_stddev << "\n";
	}

	return 0;
}
