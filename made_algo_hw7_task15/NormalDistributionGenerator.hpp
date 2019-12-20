#pragma once

#include <vector>
#include <utility>
#include <random>

// Генерирует выборку из двумерного нормального распределения с помощью преобразования Бокса-Мюллера
class NormalDistributionGenerator
{
public:
	explicit NormalDistributionGenerator();
	std::vector<std::pair<double, double>> generate_2d_sample(size_t size, double mu = 0, double sigma = 1);
	std::pair<double, double> generate_2d_point(double mu = 0, double sigma = 1);

private:
	std::default_random_engine random_engine;
	std::uniform_real_distribution<double> uniform_distribution;
};
