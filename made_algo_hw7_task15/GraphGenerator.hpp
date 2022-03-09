#pragma once

#include "NormalDistributionGenerator.hpp"

// Генерирует полный евклидов граф на плоскости в виде матрицы расстояний
class GraphGenerator
{
public:
	explicit GraphGenerator();
	std::vector<std::vector<double>> generate_complete_euclidean_graph(size_t size);

private:
	NormalDistributionGenerator normal_generator;
};