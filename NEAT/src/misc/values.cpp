#include "values.h"

namespace misc {

	int Values::INPUTS = 0;
	int Values::OUTPUTS = 0;
	int Values::POPULATION = 150;
	int Values::MAX_NOT_IMPROVED = 15;
	int Values::MIN_SIZE_COPY = 5;
	float Values::PERCENTAGE_OF_BEST = 0.85f;
	float Values::P_MUTATE_WEIGHTS = 0.80f;
	float Values::P_MUTATE_UNIFORM = 0.90f;
	float Values::P_CROSSOVER = 0.75f;
	float Values::P_ADD_NODE = 0.01f;
	float Values::P_ADD_CONNECTION = 0.30f;
	float Values::P_DISABLE_CONN = 0.000f;
	float Values::P_INTERSPECIES_MATE = 0.000f;
	float Values::P_DISABLED_GENE = 0.75f;

	const int Values::INPUT = 1;
	const int Values::BIAS = 2;
	const int Values::HIDDEN = 3;
	const int Values::OUTPUT = 4;

	float Values::RANDOM_RANGE = 2.0f;
	float Values::SIGMA = 0.5f;

	float Values::COMP_THRESHOLD = 4.0f;
	float Values::C1 = 1.0f;
	float Values::C2 = 1.0f;
	float Values::C3 = 0.30f;
	int Values::GENOME_SIZE = 20;

}