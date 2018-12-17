#pragma once

#include <array>
#include <memory>
#include <vector>

namespace genome {


	class Connection
	{
	private:
		std::shared_ptr<std::vector<std::array<int, 2>>> _innovations;
		int _in;
		int _out;
		float _weight;
		bool _enabled;
		int _innovation;
	private:
		int getInnovNum(int in, int out);
	public:
		Connection(int in, int out, float weight, bool enabled, std::shared_ptr<std::vector<std::array<int, 2>>> innovations);
		~Connection();
	public:
		inline const int& getInNode() { return _in; }
		inline const int& getOutNode() { return _out; }
		void setWeight(float weight);
		void incrementWeight(float weight);
		inline const float& getWeight() { return _weight; }
		void setEnabled(bool enabled);
		inline const bool& getEnabled() { return _enabled; }
		inline const int& getInnovation() { return _innovation; }
	};


}