#pragma once
#include <unordered_map>
typedef int const type;
typedef int const color;

typedef class piece
{
public:
	piece();
	piece(int type, int color);
	piece(char type, int color);

	bool operator==(const piece& other) const;

	type get_type() const;
	color get_color() const;

	static std::unordered_map<char, int> types;
	static std::unordered_map<char, int> colors;
private:
	int piece_type;
	int piece_color;
} p;

