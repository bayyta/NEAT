#pragma once

namespace misc {

	struct Values
	{
		static int INPUTS;					// number of inputs
		static int OUTPUTS;					// number of outputs
		static int POPULATION;				// population of network
		static int MAX_NOT_IMPROVED;		// max number of generations allowed for species not to improve its max fitness
		static int MIN_SIZE_COPY;			// min species size of copying the best genome into next generation
		static float PERCENTAGE_OF_BEST;	// min percentage of fitness from the best in each species to not be removed
		static float P_MUTATE_WEIGHTS;		// probability of weights being mutated
		static float P_MUTATE_UNIFORM;		// chance of weight being uniformly perturbed instead of assigned new random value
		static float P_CROSSOVER;			// probability of offspring from crossover
		static float P_ADD_NODE;			// probability of adding new node
		static float P_ADD_CONNECTION;		// probability of adding new connection
		static float P_DISABLE_CONN;		// probability of disabling a connection
		static float P_INTERSPECIES_MATE;	// probability of interspecies mating
		static float P_DISABLED_GENE;		// chance of inherited gene being disabled if its disabled in either parent

		static const int INPUT;				// type identifier of input node
		static const int BIAS;				// type identifier of bias node
		static const int HIDDEN;			// type identifier of hidden node
		static const int OUTPUT;			// type identifier of output node
		
		static float RANDOM_RANGE;			// range of random weight value
		static float SIGMA;					// sigma value

		static float COMP_THRESHOLD;		// compatibility threshold for species
		static float C1;					// constant for compatibility
		static float C2;
		static float C3;
		static int GENOME_SIZE;				// genome size threshold for its compatibilty
	};

}
