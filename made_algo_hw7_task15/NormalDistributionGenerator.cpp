#include "NormalDistributionGenerator.hpp"
#include <cassert>
#include <cmath>

NormalDistributionGenerator::NormalDistributionGenerator():
	uniform_distribution(std::uniform_real_distribution<double>(-1.0, 1.0))
{}

std::pair<double, double> NormalDistributionGenerator::generate_2d_point(double mu, double sigma)
{
	assert(sigma > 0);
	double x, y, radius;
	// Генерируем случайную величину radius=x^2+y^2, где x и y - независимы и распределены равномерно на [-1,1],
	// а также 0 < radius <= 1
	while (true)
	{
		x = uniform_distribution(random_engine);
		y = uniform_distribution(random_engine);
		radius = x * x + y * y;
		if (radius > 0 && radius <= 1)
			break;
	}
	double root = std::sqrt(-2 * std::log(radius) / radius);
	double z1 = mu + sigma * x * root;
	double z2 = mu + sigma * y * root;
	return std::make_pair(z1, z2);
}

std::vector<std::pair<double, double>> NormalDistributionGenerator::generate_2d_sample(size_t size, double mu, double sigma)
{
	assert(size > 0);
	assert(sigma > 0);
	std::vector<std::pair<double, double>> sample(size);
	for (size_t i = 0; i < size; ++i)
		sample[i] = generate_2d_point(mu, sigma);
	return sample;
}
