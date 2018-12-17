#include "piece.h"

namespace entity {

	Piece::Piece(const Vec2<int>& pos, const int& dir)
		: direction(dir), position(pos)
	{ }

	Piece::~Piece()
	{ }

	void Piece::move()
	{
		switch (direction)
		{
		case 0:
			position.y--;
			break;
		case 1:
			position.x++;
			break;
		case 2:
			position.y++;
			break;
		case 3:
			position.x--;
			break;
		default:
			break;
		}
	}


}