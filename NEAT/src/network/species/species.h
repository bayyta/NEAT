#pragma once

#include "../genome/genome.h"
#include "../../misc/values.h"
#include <vector>
#include <memory>
#include <cmath>


namespace species {
	using namespace genome;

	class Species
	{
	private:
		misc::Values _values;
		std::shared_ptr<Genome> _representative;
		std::vector<std::shared_ptr<Genome>> _genomes;
		std::shared_ptr<Genome> _fittestGenome;
		float _adjustedFitnessSum;
		int _notImprovedCount; // for how many generations this species has not improved its top fitness
	public:
		Species(std::shared_ptr<Genome> represntative);
		~Species();
		bool isCompatible(std::shared_ptr<Genome> g);
		void add(std::shared_ptr<Genome> genome);
		void removeWorst(const float& percentage);
		void updateFittestGenome();
		void updateAdjustedFitnessSum();
		void clear();
		std::shared_ptr<Genome> getRandomGenome(std::shared_ptr<Genome> otherThan = nullptr);
	public:
		inline void setRepresentative(std::shared_ptr<Genome> g) { _representative = g; }
		inline std::shared_ptr<Genome> getRepresentative() { return _representative; }
		inline std::shared_ptr<Genome> getFittestGenome() { return _fittestGenome; }
		inline const int& getSpeciesSize() { return _genomes.size(); }
		inline const float& getAdjustedFitnessSum() { return _adjustedFitnessSum; }
		inline const int& getNotImprovedCount() { return _notImprovedCount; }
	};



}