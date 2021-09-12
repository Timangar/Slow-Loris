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
		destination == m.destination &&
		castle == m.castle &&
		en_passant == m.en_passant &&
		promotion == m.promotion
		)
		return true;
	return false;
}
