#pragma once
#include <array>
#include <move.h>

typedef class legal_move_array
{
public:
	legal_move_array();
	legal_move_array(move i);

	legal_move_array& operator=(const legal_move_array& other);

	move operator[](unsigned i);

	void push_back(move m);
	void clear();

	const std::array<move, 128>& get_data();
	unsigned size();

	move& back();
	move& begin();

	bool empty();

private:
	unsigned _size;
	std::array<move, 128> data;
	const unsigned max_size;
} lma;

