#include "move.h"
#include <cassert>
move::move(int origin, int destination, bool castle, bool en_passant, bool promotion)
{
	this->origin = origin;
	this->destination = destination;
	this->castle = castle;
	this->en_passant = en_passant;
	this->promotion = promotion;
}

bool move::operator==(const move& m) const
{
	if (
		origin == m.origin &&
		destination == m.destination
		) {
		return true;
	}
	return false;
}

bool move::operator<(const move& m) const
{
	if (origin < m.origin)
		return true;
	else if (origin == m.origin)
		return (destination < m.destination);
	else
		return false;
}
