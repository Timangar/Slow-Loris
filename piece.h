#pragma once
#include <unordered_map>
typedef unsigned int const type;
typedef int const color;

typedef class piece
{
public:
	piece();
	piece(int type, int color);
	piece(char type, int color);

	type get_type() const;
	color get_color() const;

	static std::unordered_map<char, int> types;
	static std::unordered_map<char, int> colors;
private:
	unsigned int piece_type;
	int piece_color;
} p;

