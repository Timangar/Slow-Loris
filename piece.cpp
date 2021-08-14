#include "piece.h"
#include <cassert>

std::unordered_map<char, int>  piece::types = {
	{'E', 0},	//empty
	{'K', 1},	//king
	{'Q', 2},	//queen
	{'B', 3},	//bishop
	{'N', 4},	//knight
	{'R', 5},	//rook
	{'P', 6}	//pawn
};

std::unordered_map<char, int> piece::colors = {
	{'w', 1},
	{'W', 1},
	{'b', -1},
	{'B', -1}
};

piece::piece()
{
	piece_type = 0;
	piece_color = 0;
}

piece::piece(int type, int color)
{
	piece_type = type;
	piece_color = color;
	assert(piece_type < 7);
	assert(color <= 1 && color >= -1);
}

piece::piece(char type, int color)
{
	piece_type = types.at(type);
	piece_color = color;
	assert(piece_type < 7);
	assert(color <= 1 && color >= -1);
}

type piece::get_type() const
{
	assert(piece_type < 7);
	return type(piece_type);
}
color piece::get_color() const
{
	assert(piece_color >= -1 && piece_color <= 1);
	return color(piece_color);
}