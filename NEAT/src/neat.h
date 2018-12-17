#pragma once

#include <random>
#include <time.h>
#include "network/species/species.h"
#include "misc/values.h"

namespace neat {

	class Neat
	{
	private:
		bool _initialized;
		int _generation; // generation count
		std::shared_ptr<genome::Genome> _fittestGenome;
		std::vector<std::shared_ptr<genome::Genome>> _genomes;
		std::vector<std::shared_ptr<species::Species>> _species;
		std::shared_ptr<std::vector<std::array<int, 2>>> _innovations;
		std::shared_ptr<std::default_random_engine> _dre;
		std::shared_ptr<std::normal_distribution<float>> _nd;
	public:
		misc::Values values;
	public:
		Neat();
		~Neat();
		void init();
		void restart();
		void feedForward();
		void setInput(const float& value, const int& inputIndex, const int& genomeIndex);
		void updateOutput(const int& genomeIndex);
		const float& getOutput(const int& outputIndex, const int& genomeIndex);
		void setFitness(const float& fitness, const int& genomeIndex);
		void printGenome(const std::shared_ptr<genome::Genome>& genome);
	public:
		const std::shared_ptr<genome::Genome>& getGenome(const int& genomeIndex);
		const std::shared_ptr<genome::Genome>& getFittestGenome();
		const int& getGeneration();
		const int getSpeciesCount();
		const int getPopulationCount();
		const bool& initialized();
	};


}