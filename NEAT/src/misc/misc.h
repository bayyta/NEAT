#pragma once

#include <math.h>

namespace misc {

	static float sigmoid(const float &x)
	{
		return 1.0f / (1.0f + exp((double)-x * 4.9));;
	}

}