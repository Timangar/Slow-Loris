#include "move.h"

move::move(int origin, int destination, bool castle, bool en_passant, bool promotion)
{
	this->origin = origin;
	this->destination = destination;
	this->castle = castle;
	this->en_passant = en_passant;
	this->promotion = promotion;
}

bool move::operator==(move& m)
{
	if (origin == m.origin
		&& destination == m.destination)
	{
		m = *this;
		return true;
	}
	else
		return false;
}
