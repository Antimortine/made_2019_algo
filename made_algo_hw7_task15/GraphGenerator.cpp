#include "GraphGenerator.hpp"
#include <cassert>
#include <cmath>

GraphGenerator::GraphGenerator()
{}

std::vector<std::vector<double>> GraphGenerator::generate_complete_euclidean_graph(size_t size)
{
	assert(size > 0);
	std::vector<std::vector<double>> matrix(size, std::vector<double>(size));
	std::vector<std::pair<double, double>> points = normal_generator.generate_2d_sample(size);

	for (size_t i = 0; i < size; ++i)
		for (size_t j = 0; j < size; ++j)
		{
			std::pair<double, double> p1 = points[i];
			std::pair<double, double> p2 = points[j];
			double delta_x = p2.first - p1.first;
			double delta_y = p2.second - p1.second;
			matrix[i][j] = std::sqrt(delta_x * delta_x + delta_y * delta_y);
		}

	return matrix;
}
