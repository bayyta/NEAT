#pragma once

#include "connection.h"
#include <vector>
#include <algorithm>
#include <memory>

namespace genome {

	class Node
	{
	private:
		int _id;
		int _layer;
		int _type;
		float _value;
		std::vector<std::shared_ptr<Connection>> _inConnections;
	public:
		Node(int id, int layer, int type);
		Node(const Node& node);
		~Node();
	public:
		void clear();
		void removeConnection(std::shared_ptr<Connection> c);
		inline const std::vector<std::shared_ptr<Connection>>& getInConnections() const { return _inConnections; }
		inline void addInConnection(std::shared_ptr<Connection> c) { _inConnections.push_back(c); }
		void setId(int id);
		inline const int& getId() const { return _id; }
		inline void setLayer(int layer) { _layer = layer; }
		inline const int& getLayer() const { return _layer; }
		inline void incrementLayer(int layer) { _layer += layer; }
		inline const int& getType() const { return _type; }
		inline void setValue(float value) { _value = value; }
		inline const float& getValue() const { return _value; }
		inline void incrementValue(float value) { _value += value; }
	};


}