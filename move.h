#pragma once
#define RANK 8
#define FILE__ 1
#define BLACK -1
#define WHITE 1

struct move
{
	move(int origin, int destination, bool castle = false, bool en_passant = false, bool promotion = false);

	bool operator==(move& m);

	int origin;
	int destination;

	bool castle;
	bool en_passant;
	bool promotion;
};
