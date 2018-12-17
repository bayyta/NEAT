#pragma once

#include <SDL.h>
#include "../../misc/vec2.h"
#include "../../misc/input.h"

namespace entity {

	using namespace math;

	class Piece
	{
	public:
		Vec2<int> position;
		int direction;
	public:
		Piece(const Vec2<int>& pos, const int& dir);
		~Piece();
		void move();
	};


}