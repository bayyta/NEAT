#pragma once

#include "node.h"
#include "connection.h"
#include "../../misc/misc.h"
#include "../../misc/values.h"
#include <cstdio>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <array>
#include <memory>
#include <random>
#include <stdexcept>
#include <deque>

namespace genome {

	using String = std::string;

	class Genome : public std::enable_shared_from_this<Genome>
	{
	private:
		misc::Values _values;
		float _fitness;
		std::vector<std::shared_ptr<Node>> _nodes;
		std::vector<std::shared_ptr<Connection>> _connections;
		// for easier access
		std::vector<std::shared_ptr<Node>> _nodesInput;
		std::vector<std::shared_ptr<Node>> _nodesOutput;
		///
		/// could also reset innovations after each generation - it would be the same
		///
		std::shared_ptr<std::vector<std::array<int, 2>>> _globalInnovations;
	private:
		void initNodes();
		void initConnections();
		float getRandomWeight();
		void getConnectingNodes(std::vector<int>& nodeIDs);
		void getNodesOfHigherLayer(std::vector<std::shared_ptr<Node>>& nodes, const int& layer);
		void addConnection(std::shared_ptr<Connection> c);
		void updateNodes();
	public:
		const int inputCount;
		const int biasCount;
		const int outputCount;
	public:
		Genome(int inputs, int outputs, std::shared_ptr<std::vector<std::array<int, 2>>> innovations, bool empty = false);
		Genome(const Genome& genome);
		~Genome();
		void mutateAddNode();
		void mutateAddConnection();
		void mutateWeights(const std::shared_ptr<std::default_random_engine> &e, const std::shared_ptr<std::normal_distribution<float>> &nd);
		void mutateDisableConnection();
		std::shared_ptr<Genome> crossover(const std::shared_ptr<Genome> &g);
		std::shared_ptr<Connection> getConnection(const int& innovation);
		void updateOutput();
		void print();
	public:
		void setInput(const float& value, const int& index);
		const float& getOutput(const int& index) const;
		inline void setFitness(const float& f) { _fitness = f; }
		inline const float& getFitness() const { return _fitness; }
		inline int getConnectionCount() const { return _connections.size(); }
		const std::vector<std::shared_ptr<Node>>& getNodes() const;
		const int& getHiddenNodeCount() const;
		inline const std::vector<std::shared_ptr<Connection>>& getConnections() const { return _connections; }
		inline int getInnovationCount() const { return _globalInnovations->size(); }
		std::shared_ptr<std::vector<std::array<int, 2>>> getInnovations() const;
	};



}