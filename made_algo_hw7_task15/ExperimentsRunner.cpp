#include "ExperimentsRunner.hpp"
#include <cassert>
#include <cmath>

ExperimentResult::ExperimentResult(size_t graph_size, size_t trials, double approximation_ratio_mean, double approximation_ratio_stddev)
	: graph_size(graph_size),
	trials(trials),
	approximation_ratio_mean(approximation_ratio_mean),
	approximation_ratio_stddev(approximation_ratio_stddev)
{}

ExperimentResult ExperimentsRunner::run_experiment(size_t graph_size, size_t trials)
{
	assert(graph_size > 1);
	assert(trials > 1);
	std::vector<double> approximation_ratios;
	for (size_t i = 0; i < trials; ++i)
	{
		std::vector<std::vector<double>> graph = generator.generate_complete_euclidean_graph(graph_size);
		double min_weight = BruteForceTSPSolver::get_shortest_cycle_cost(graph);
		double approx_min_weight = MSTApproximateTSPSolver::approximate_shortest_cycle_cost(graph);
		double approximation_ratio = approx_min_weight / min_weight;
		approximation_ratios.push_back(approximation_ratio);
	}
	double mean = 0;
	for (double ratio : approximation_ratios)
		mean += ratio;
	mean /= trials;

	double variance = 0;
	for (double ratio : approximation_ratios)
	{
		double delta = ratio - mean;
		variance += delta * delta;
	}
	variance /= (trials - 1);
	double stddev = std::sqrt(variance);
	return ExperimentResult(graph_size, trials, mean, variance);
}
