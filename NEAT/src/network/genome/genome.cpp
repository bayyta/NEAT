#include "genome.h"

namespace genome {

	Genome::Genome(int inputs, int outputs, std::shared_ptr<std::vector<std::array<int, 2>>> innovations, bool empty)
		: inputCount(inputs), biasCount(1), outputCount(outputs), _globalInnovations(innovations)
	{
		_fitness = 0.0f;

		initNodes();
		if (!empty)
			initConnections();
	}

	Genome::Genome(const Genome& genome)
		: inputCount(genome.inputCount), biasCount(genome.biasCount), outputCount(genome.outputCount)
	{
		_fitness = genome.getFitness();

		for (auto& n : genome.getNodes())
		{
			std::shared_ptr<Node> newNode = std::make_shared<Node>(*n);
			_nodes.push_back(newNode);
			if (n->getType() == _values.INPUT)
				_nodesInput.push_back(newNode);
			else if (n->getType() == _values.OUTPUT)
				_nodesOutput.push_back(newNode);
		}

		for (auto& c : genome.getConnections())
		{
			std::shared_ptr<Connection> copy = std::make_shared<Connection>(*c);
			_connections.push_back(copy);
		}

		_globalInnovations = genome.getInnovations();
	}

	void Genome::initNodes()
	{
		for (int i = 0; i < inputCount; i++)
		{
			std::shared_ptr<Node> n = std::make_shared<Node>(_nodes.size(), 0, _values.INPUT);
			_nodes.push_back(n);
			_nodesInput.push_back(n);
		}

		for (int i = 0; i < biasCount; i++)
		{
			std::shared_ptr<Node> n = std::make_shared<Node>(_nodes.size(), 0, _values.BIAS);
			n->setValue(1.0f);
			_nodes.push_back(n);
		}

		for (int i = 0; i < outputCount; i++)
		{
			std::shared_ptr<Node> n = std::make_shared<Node>(_nodes.size(), 1, _values.OUTPUT);
			_nodes.push_back(n);
			_nodesOutput.push_back(n);
		}
	}

	void Genome::initConnections()
	{
		// add connection for every input/bias to random output
		std::vector<std::shared_ptr<Node>> nodesOut;
		getNodesOfHigherLayer(nodesOut, 0);
		for (int i = 0; i < inputCount + biasCount; i++)
		{
			std::shared_ptr<Node> nodeOut = nodesOut.at(rand() % nodesOut.size());
			int out = nodeOut->getId();
			float weight = getRandomWeight();
			std::shared_ptr<Connection> c = std::make_shared<Connection>(i, out, weight, true, _globalInnovations);
			_connections.push_back(c);
			// also add to node for later calculation of outputs
			nodeOut->addInConnection(c);
		}
	}

	Genome::~Genome()
	{ }

	void Genome::getConnectingNodes(std::vector<int>& nodeIDs)
	{
		for (int i = 0; i < nodeIDs.size(); i++)
			for (auto& c : _connections)
				if (c->getInNode() == nodeIDs.at(i))
					if (std::find(nodeIDs.begin(), nodeIDs.end(), c->getOutNode()) == nodeIDs.end())
						nodeIDs.push_back(c->getOutNode());
	}

	void Genome::getNodesOfHigherLayer(std::vector<std::shared_ptr<Node>>& nodes, const int& layer)
	{
		for (auto& n : _nodes)
			if (n->getLayer() > layer && n->getType() != _values.OUTPUT)
				nodes.push_back(n);

		// always include output nodes even though they might not have higher layer
		for (auto& n : _nodesOutput)
			nodes.push_back(n);
	}

	float Genome::getRandomWeight()
	{
		return -_values.RANDOM_RANGE + (float)rand() / ((float)(RAND_MAX / (_values.RANDOM_RANGE - (-_values.RANDOM_RANGE))));
	}

	void Genome::mutateAddNode()
	{
		// select random connection to add node and disable it
		std::shared_ptr<Connection> cPrev = _connections.at(rand() % _connections.size());
		cPrev->setEnabled(false);

		// add new node and connections
		std::shared_ptr<Node> newNode = std::make_shared<Node>(_nodes.size(), _nodes.at(cPrev->getInNode())->getLayer() + 1, _values.HIDDEN);
		_nodes.push_back(newNode);
		std::shared_ptr<Connection> cNew1 = std::make_shared<Connection>(cPrev->getInNode(), newNode->getId(), 1.0f, true, _globalInnovations);
		std::shared_ptr<Connection> cNew2 = std::make_shared<Connection>(newNode->getId(), cPrev->getOutNode(), cPrev->getWeight(), true, _globalInnovations);
		_connections.push_back(cNew1);
		_connections.push_back(cNew2);

		// also add connections to nodes for later calculation of outputs
		std::shared_ptr<Node> prevNodeOut = _nodes.at(cPrev->getOutNode());
		newNode->addInConnection(cNew1);
		prevNodeOut->addInConnection(cNew2);

		updateNodes();
	}

	void Genome::mutateAddConnection()
	{
		int in;
		int out;
		float weight;

		// create new list with only non-ouput nodes
		std::vector<std::shared_ptr<Node>> nodesIn;
		for (auto& n : _nodes)
			if (n->getType() != _values.OUTPUT)
				nodesIn.push_back(n);

		// select random in-node
		std::shared_ptr<Node> nodeIn = nodesIn.at(rand() % nodesIn.size());
		in = nodeIn->getId();

		// select random out-node
		std::vector<std::shared_ptr<Node>> nodesOut;
		getNodesOfHigherLayer(nodesOut, nodeIn->getLayer());
		std::shared_ptr<Node> nodeOut = nodesOut.at(rand() % nodesOut.size());
		out = nodeOut->getId();

		// check if connection muation already exists in genome, [if so change set enable]
		for (auto& c : _connections)
		{
			if (c->getInNode() == in && c->getOutNode() == out)
			{
				c->setEnabled(true);
				//c->setWeight(getRandomWeight());
				return;
			}
		}

		// randomize weight
		weight = getRandomWeight();

		// add connection - enabled is always set to true
		std::shared_ptr<Connection> c = std::make_shared<Connection>(in, out, weight, true, _globalInnovations);
		_connections.push_back(c);
		// also add to node for later calculation of outputs
		nodeOut->addInConnection(c);

		if (nodeIn->getLayer() >= nodeOut->getLayer())
		{
			updateNodes();
		}
	}

	void Genome::mutateWeights(const std::shared_ptr<std::default_random_engine>& e, const std::shared_ptr<std::normal_distribution<float>>& nd)
	{
		for (auto& c : _connections)
		{
			// P_MUTATEUNIFORM % chance of weight being uniformly perturbed
			if ((rand() % 100 + 1) <= _values.P_MUTATE_UNIFORM * 100)
			{
				c->incrementWeight((*nd)(*e));
			}
			// (100 - P_MUTATEUNIFORM) % chance of weight being assigned a random new value
			else
			{
				c->setWeight(getRandomWeight());
			}
		}
	}

	void Genome::mutateDisableConnection()
	{
		int r = rand() % _connections.size();
		_connections.at(r)->setEnabled(false);
	}

	std::shared_ptr<Genome> Genome::crossover(const std::shared_ptr<Genome> &g)
	{
		// new child genome
		std::shared_ptr<Genome> child = std::make_shared<Genome>(inputCount, outputCount, _globalInnovations, true);

		// select most and least fit parent
		std::shared_ptr<Genome> mostFitParent = nullptr;
		std::shared_ptr<Genome> leastFitParent = nullptr;
		if (getFitness() > g->getFitness())
		{
			mostFitParent = shared_from_this();
			leastFitParent = g;
		}
		else if (getFitness() < g->getFitness())
		{
			mostFitParent = g;
			leastFitParent = shared_from_this();
		}

		// same fitness parents
		if (mostFitParent == nullptr)
		{
			for (int i = 0; i < _globalInnovations->size(); i++)
			{
				std::shared_ptr<Connection> newConn;
				std::shared_ptr<Connection> p1, p2;
				if (rand() % 2 == 0)
				{
					p1 = getConnection(i);
					p2 = g->getConnection(i);
				}
				else
				{
					p1 = g->getConnection(i);
					p2 = getConnection(i);
				}
				if (p1 == nullptr)
					continue;
				newConn = std::make_shared<Connection>(*p1);

				// check if either parents has a disabled gene, if so child gene has P_DISABLEDGENE % 
				// chance of also being disabled
				if (p2 != nullptr)
				{
					if (p2->getEnabled() == false)
					{
						if (((rand() % 100) + 1) <= _values.P_DISABLED_GENE * 100)
						{
							newConn->setEnabled(false);
						}
					}
				}

				child->addConnection(newConn);

			}
		}
		// different fitness parents
		else
		{
			for (auto& connMostFitParent : mostFitParent->_connections)
			{
				std::shared_ptr<Connection> connLeastFitParent = nullptr;
				for (auto& c2 : leastFitParent->_connections)
				{
					// check if least fit parent has the same gene
					if (c2->getInnovation() == connMostFitParent->getInnovation())
					{
						connLeastFitParent = c2;
						break;
					}
				}

				if (connLeastFitParent == nullptr)
				{
					// the most fit parent had excess/disjoint gene, add gene copy to child
					child->addConnection(std::make_shared<Connection>(*connMostFitParent));
				}
				else
				{
					// both parents had the same gene, randomize the outcome and add copy to child
					std::shared_ptr<Connection> newConn;
					if (rand() % 2 == 0)
						newConn = std::make_shared<Connection>(*connMostFitParent);
					else
						newConn = std::make_shared<Connection>(*connLeastFitParent);

					// check if either/both parents has a disabled gene, if so child gene has P_DISABLEDGENE % chance of also 
					// being disabled
					if (connMostFitParent->getEnabled() == false || connLeastFitParent->getEnabled() == false)
					{
						if (((rand() % 100) + 1) <= _values.P_DISABLED_GENE * 100)
						{
							newConn->setEnabled(false);
						}
					}

					child->addConnection(newConn);
				}
			}
		}

		child->updateNodes();

		return child;
	}

	void Genome::addConnection(std::shared_ptr<Connection> conn)
	{
		int highestId = conn->getInNode() > conn->getOutNode() ? conn->getInNode() : conn->getOutNode();

		// if nodes of new connection doesn't exist, create them
		while (highestId >= _nodes.size())
		{
			_nodes.push_back(std::make_shared<Node>(_nodes.size(), 0, _values.HIDDEN));
		}

		// make sure there's no redundancy
		std::vector<int> ntu{ conn->getOutNode() };
		getConnectingNodes(ntu);
		if (std::find(ntu.begin(), ntu.end(), conn->getInNode()) != ntu.end())
		{
			return;
		}

		// add connection
		_connections.push_back(conn);
	}

	void Genome::updateNodes()
	{
		// go through every node and update its layer and in-connections - input nodes dont need update
		// hence why i = inputCount
		for (int i = inputCount; i < _nodes.size(); i++)
		{
			_nodes.at(i)->clear();

			int layer = 0;
			std::deque<int> connectedNodes;

			for (auto& c : _connections)
			{
				if (c->getOutNode() == _nodes.at(i)->getId())
				{
					_nodes.at(i)->addInConnection(c);
					connectedNodes.push_back(c->getInNode());
				}
			}

			while (connectedNodes.size() > 0)
			{
				layer++;
				const int prevSize = connectedNodes.size();
				for (int i = 0; i < prevSize; i++)
				{
					for (auto& c : _connections)
					{
						if (c->getOutNode() == connectedNodes.at(i))
							if (std::find(connectedNodes.begin() + prevSize, connectedNodes.end(), c->getInNode()) == connectedNodes.end())
								connectedNodes.push_back(c->getInNode());
					}
				}
				for (int i = 0; i < prevSize; i++)
					connectedNodes.pop_front();
			}
			_nodes.at(i)->setLayer(layer);
		}
	}

	std::shared_ptr<Connection> Genome::getConnection(const int& innovation)
	{
		for (auto& c : _connections)
		{
			if (c->getInnovation() == innovation)
				return c;
		}

		return nullptr;
	}

	void Genome::updateOutput()
	{
		// number of nodes with specific layer, break when nodes = 0
		int count = 0;
		// start at layer 1, input layer (0) is already set
		int layer = 1;
		while (true)
		{
			count = 0;
			for (auto& n : _nodes)
			{
				if (n->getLayer() == layer)
				{
					count++;
					// first reset value from possible previous calculation
					n->setValue(0.0f);
					// add weights * activations to value
					for (auto& c : n->getInConnections())
					{
						if (c->getEnabled() == false)
							continue;
						n->incrementValue(_nodes.at(c->getInNode())->getValue() * c->getWeight());
					}
					// add sigmoid function to node value
					n->setValue(misc::sigmoid(n->getValue()));
				}
			}
			if (count == 0)
				break;
			layer++;
		}
	}

	void Genome::print()
	{
		printf("------Genome------\nFitness: %f\n", _fitness);
		for (auto& c : _connections)
		{
			printf("Innov %d: %d->%d, weight: %f enabled: %s\n", c->getInnovation(),
				c->getInNode(), c->getOutNode(), c->getWeight(), c->getEnabled() ? "true" : "false");
		}
		printf("------------------\n");
	}

	std::shared_ptr<std::vector<std::array<int, 2>>> Genome::getInnovations() const
	{
		return _globalInnovations;
	}

	void Genome::setInput(const float& value, const int& index)
	{
		_nodesInput.at(index)->setValue(value);
	}

	const float& Genome::getOutput(const int& index) const
	{
		return _nodesOutput.at(index)->getValue();
	}

	const std::vector<std::shared_ptr<Node>>& Genome::getNodes() const
	{
		return _nodes;
	}

	const int& Genome::getHiddenNodeCount() const
	{
		return _nodes.size() - inputCount - biasCount - outputCount;
	}





}