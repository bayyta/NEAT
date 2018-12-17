
#include "species.h"

namespace species {

	Species::Species(std::shared_ptr<Genome> representative)
		: _representative(representative)
	{
		_notImprovedCount = 0;
		_adjustedFitnessSum = 0.0f;
		_genomes.push_back(representative);
	}

	Species::~Species()
	{ }

	bool Species::isCompatible(std::shared_ptr<Genome> g)
	{
		int n = 1;
		int smallerGenomeSize = _representative->getConnectionCount() < g->getConnectionCount() ? _representative->getConnectionCount() : g->getConnectionCount();
		if (smallerGenomeSize > _values.GENOME_SIZE)
			n = smallerGenomeSize == g->getConnectionCount() ? _representative->getConnectionCount() : smallerGenomeSize;

		int matchingGeneCount = 0;
		int excessAndDisjointCount = 0;
		float avaregeWeightDiff = 0.0f;

		const int SIZE = _representative->getInnovationCount();
		// true if genome has corresponding innovation
		bool *i1 = new bool[SIZE] {};
		bool *i2 = new bool[SIZE] {};

		int max1 = 0;
		int max2 = 0;
		for (auto& c : _representative->getConnections())
		{
			if (c->getInnovation() > max1)
			{
				max1 = c->getInnovation();
			}
			i1[c->getInnovation()] = true;
		}
		for (auto& c : g->getConnections())
		{
			if (c->getInnovation() > max2)
			{
				max2 = c->getInnovation();
			}
			i2[c->getInnovation()] = true;

			// genomes have same gene, add weight difference to avaregeWeightDiff
			if (i1[c->getInnovation()])
			{
				matchingGeneCount++;
				avaregeWeightDiff += abs(c->getWeight() - _representative->getConnection(c->getInnovation())->getWeight());
			}
		}

		avaregeWeightDiff /= matchingGeneCount;

		// loop from 0 to the highest innovation number on either genome
		for (int i = 0; i <= (max1 > max2 ? max1 : max2); i++)
		{
			// if only one genome has the gene, add to excessAndDisjointCount
			excessAndDisjointCount += (i1[i] ^ i2[i]);
		}

		delete[] i1;
		delete[] i2;

		// distance measure
		float dm = ((_values.C1 * excessAndDisjointCount) / n) + _values.C3 * avaregeWeightDiff;

		return dm < _values.COMP_THRESHOLD;
	}

	void Species::add(std::shared_ptr<Genome> genome)
	{
		_genomes.push_back(genome);
	}

	void Species::removeWorst(const float& percentage)
	{
		_genomes.erase(std::remove_if(_genomes.begin(), _genomes.end(),
			[&f = _fittestGenome, &p = percentage](const auto& g)
		{ return g->getFitness() < f->getFitness() * p; }),
			_genomes.end());
	}

	void Species::updateFittestGenome()
	{
		if (_genomes.size() == 0)
		{
			_fittestGenome = nullptr;
			return;
		}
		_notImprovedCount++;
		_fittestGenome = _genomes.at(0);
		for (auto& g : _genomes)
		{
			if (g->getFitness() > _fittestGenome->getFitness())
			{
				_notImprovedCount = 0;
				_fittestGenome = g;
			}
		}
	}

	void Species::updateAdjustedFitnessSum()
	{
		_adjustedFitnessSum = 0.0f;
		if (_genomes.size() == 0)
			return;
		for (std::shared_ptr<Genome> genome : _genomes)
		{
			_adjustedFitnessSum += genome->getFitness();
		}
		_adjustedFitnessSum /= _genomes.size();
	}

	void Species::clear()
	{
		_genomes.clear();
	}

	std::shared_ptr<Genome> Species::getRandomGenome(std::shared_ptr<Genome> otherThan)
	{
		if (otherThan == nullptr)
			return _genomes.at(rand() % _genomes.size());
		if (_genomes.size() <= 1)
			return otherThan;
		std::vector<std::shared_ptr<Genome>> temp = _genomes;
		temp.erase(std::remove(temp.begin(), temp.end(), otherThan));
		return temp.at(rand() % temp.size());
	}

}