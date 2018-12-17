#include "neat.h"

namespace neat {

	Neat::Neat()
	{
		srand(time(NULL));
		_initialized = false;
	}

	Neat::~Neat()
	{ }


	void Neat::init()
	{
		if (!_initialized)
		{
			_innovations = std::make_shared<std::vector<std::array<int, 2>>>();
			_dre = std::make_shared<std::default_random_engine>();
			_nd = std::make_shared<std::normal_distribution<float>>(0.0f, values.SIGMA);
			_initialized = true;
		}

		_generation = 0;
		_fittestGenome = nullptr;

		for (int i = 0; i < values.POPULATION; i++)
		{
			_genomes.push_back(std::make_shared<genome::Genome>(values.INPUTS, values.OUTPUTS, _innovations));
		}
	}

	void Neat::restart()
	{
		_genomes.clear();
		_species.clear();
		_innovations->clear();
		init();
	}

	void Neat::feedForward()
	{
		_generation++;

		// assign genomes to species
		for (auto& genome : _genomes)
		{
			bool assigned = false;
			for (auto& species : _species)
			{
				if (species->isCompatible(genome))
				{
					species->add(genome);
					assigned = true;
					break;
				}
			}

			if (!assigned)
			{
				_species.push_back(std::make_shared<species::Species>(genome));
			}

			// update fittest genome of current run
			if (_fittestGenome != nullptr)
			{
				if (genome->getFitness() > _fittestGenome->getFitness())
				{
					_fittestGenome = genome;
				}
			}
			else
			{
				_fittestGenome = genome;
			}
		}

		// after genomes assigned to species clear list
		_genomes.clear();

		// calculate adjusted fitness for each species
		float totalAdjustedFitnessSum = 0.0f;

		for (auto& species : _species)
		{
			species->updateAdjustedFitnessSum();
			totalAdjustedFitnessSum += species->getAdjustedFitnessSum();
		}

		// breed through muation/crossover
		for (auto species = _species.begin(); species != _species.end();)
		{
			(*species)->updateFittestGenome();

			int offspring = -1;

			/* if total adjusted fittness sum is 0 give every genome 1 offspring - avoid division by
			totalAdjustedFitnessSum since it's zero */
			if (totalAdjustedFitnessSum == 0)
			{
				offspring = values.POPULATION / _species.size();
			}
			else
			{
				offspring = ((*species)->getAdjustedFitnessSum() / totalAdjustedFitnessSum) * values.POPULATION;
			}

			// remove species if not improved for MAX_NOT_IMPROVED number of generations
			if ((*species)->getNotImprovedCount() >= values.MAX_NOT_IMPROVED
				|| offspring == 0 || (*species)->getSpeciesSize() == 0)
			{
				totalAdjustedFitnessSum -= (*species)->getAdjustedFitnessSum();
				species = _species.erase(species);
				continue;
			}

			/* add the best performing from each species to current generation if species size > MIN_SIZE_COPY,
			remove the worst performing genomes and add new random genome in that species as
			representative for next generation */
			if ((*species)->getSpeciesSize() > values.MIN_SIZE_COPY)
				_genomes.push_back(std::make_shared<genome::Genome>(*((*species)->getFittestGenome())));

			(*species)->removeWorst(values.PERCENTAGE_OF_BEST);
			(*species)->setRepresentative((*species)->getRandomGenome());

			// breed according to how much offspring each species gets
			for (int i = 0; i < offspring; i++)
			{
				std::shared_ptr<genome::Genome> child = (*species)->getRandomGenome();
				bool mutation = false;
				// P_CROSSOVER % chance of crossover
				if ((rand() % 1000) + 1 < values.P_CROSSOVER * 1000.0f)
				{
					child = child->crossover((*species)->getRandomGenome(child));
					mutation = true;
				}
				else if ((rand() % 1000) + 1 <= (1.0f / (1.0f - values.P_CROSSOVER)) * values.P_INTERSPECIES_MATE * 1000.0f)
				{
					std::shared_ptr<species::Species> randSpecies = _species.at(rand() % _species.size());
					if (randSpecies->getSpeciesSize() > 0)
					{
						child = child->crossover(randSpecies->getRandomGenome());
						mutation = true;
					}
				}
				else
				{
					// no crossover so child is not copied as in crossover() function - make copy of itself
					child = std::make_shared<genome::Genome>(*child);
				}

				if ((rand() % 1000) + 1 <= values.P_ADD_CONNECTION * 1000.0f)
				{
					mutation = true;
					child->mutateAddConnection();
				}
				if ((rand() % 1000) + 1 <= values.P_ADD_NODE * 1000.0f)
				{
					mutation = true;
					child->mutateAddNode();
				}
				if ((rand() % 1000) + 1 <= values.P_MUTATE_WEIGHTS * 1000.0f)
				{
					mutation = true;
					child->mutateWeights(_dre, _nd);
				}
				if ((rand() % 1000) + 1 <= values.P_DISABLE_CONN * 1000.0f)
				{
					mutation = true;
					child->mutateDisableConnection();
				}
				if (!mutation)
					child->mutateWeights(_dre, _nd);

				// add to new population
				_genomes.push_back(child);
			}

			species++;
		}

		// clear genomes in all species to prepare for new generation
		for (auto& s : _species)
			s->clear();

	}

	void Neat::setInput(const float& value, const int& inputIndex, const int& genomeIndex)
	{
		_genomes[genomeIndex]->setInput(value, inputIndex);
	}

	void Neat::updateOutput(const int& genomeIndex)
	{
		_genomes[genomeIndex]->updateOutput();
	}

	const float& Neat::getOutput(const int& outputIndex, const int& genomeIndex)
	{
		return _genomes[genomeIndex]->getOutput(outputIndex);
	}

	void Neat::setFitness(const float& fitness, const int& genomeIndex)
	{
		_genomes[genomeIndex]->setFitness(fitness);
	}

	void Neat::printGenome(const std::shared_ptr<genome::Genome>& genome)
	{
		genome->print();
	}

	const std::shared_ptr<genome::Genome>& Neat::getGenome(const int& genomeIndex)
	{
		return _genomes[genomeIndex];
	}

	const std::shared_ptr<genome::Genome>& Neat::getFittestGenome()
	{
		return _fittestGenome;
	}

	const int& Neat::getGeneration()
	{
		return _generation;
	}

	const int Neat::getSpeciesCount()
	{
		return _species.size();
	}

	const int Neat::getPopulationCount()
	{
		return _genomes.size();
	}

	const bool& Neat::initialized()
	{
		return _initialized;
	}

}