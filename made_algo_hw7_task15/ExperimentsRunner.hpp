#pragma once

#include "GraphGenerator.hpp"
#include "BruteForceTSPSolver.hpp"
#include "MSTApproximateTSPSolver.hpp"
#include "Edge.hpp"

struct ExperimentResult
{
	explicit ExperimentResult(size_t graph_size, size_t trials, double approximation_ratio_mean, double approximation_ratio_stddev);
	size_t graph_size;
	size_t trials;
	double approximation_ratio_mean;
	double approximation_ratio_stddev;
};

class ExperimentsRunner
{
public:
	explicit ExperimentsRunner() {};
	ExperimentResult run_experiment(size_t graph_size, size_t trials);

private:
	GraphGenerator generator;
};
