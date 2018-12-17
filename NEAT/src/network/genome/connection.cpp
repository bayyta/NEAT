#include "connection.h"

namespace genome {


	Connection::Connection(int in, int out, float weight, bool enabled, std::shared_ptr < std::vector<std::array<int, 2>>> innovations)
		: _in(in), _out(out), _weight(weight), _enabled(enabled), _innovations(innovations)
	{
		_innovation = getInnovNum(in, out);
	}

	Connection::~Connection()
	{ }

	int Connection::getInnovNum(int in, int out)
	{
		int innov = -1;

		// check if innovation already exists
		for (int i = 0; i < _innovations->size(); i++)
		{
			std::array<int, 2> &l = _innovations->at(i);
			if (l.at(0) == in && l.at(1) == out)
			{
				innov = i;
			}
		}

		// doesn't exist, add new
		if (innov < 0)
		{
			innov = _innovations->size();
			_innovations->push_back({ in, out });
		}

		return innov;
	}

	void Connection::setWeight(float weight)
	{
		_weight = weight;
	}

	void Connection::incrementWeight(float weight)
	{
		_weight += weight;
	}

	void Connection::setEnabled(bool enabled)
	{
		_enabled = enabled;
	}





}