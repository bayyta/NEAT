#include "node.h"

namespace genome {

	Node::Node(int id, int layer, int type)
		: _id(id), _layer(layer), _type(type)
	{
		_value = 0.0f;
	}

	Node::Node(const Node& node)
	{
		_id = node.getId();
		_layer = node.getLayer();
		_type = node.getType();
		_value = node.getValue();
		for (auto c : node.getInConnections())
		{
			_inConnections.push_back(std::make_shared<Connection>(*c));
		}
	}

	Node::~Node() {}

	void Node::clear()
	{
		_layer = 0;
		_inConnections.clear();
	}

	void Node::removeConnection(std::shared_ptr<Connection> c)
	{
		_inConnections.erase(std::remove(_inConnections.begin(), _inConnections.end(), c));
	}

	void Node::setId(int id)
	{
		_id = id;
	}

}