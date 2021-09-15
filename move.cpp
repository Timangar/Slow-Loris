#include "move.h"
#include <cassert>
move::move()
{
	origin = -1;
	destination = -1;
	castle = 0;
	en_passant = 0;
	promotion = 0;
}
move::move(int origin, int destination, bool castle, bool en_passant, bool promotion)
{
	this->origin = origin;
	this->destination = destination;
	this->castle = castle;
	this->en_passant = en_passant;
	this->promotion = promotion;
}

bool move::operator==(move& m) const
{
	if (
		origin == m.origin &&
		destination == m.destination
		) {
		m = *this;
		return true;
	}
	return false;
}
