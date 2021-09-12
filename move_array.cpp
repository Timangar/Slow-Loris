#include "move_array.h"

std::array<int, 7> const move_array::clear_arr = { -1, -1, -1, -1, -1, -1, -1 };

move_array::move_array()
{
	data = clear_arr;
	size = 0;
}

move_array::move_array(int i)
{
	data = clear_arr;
	data[0] = i;
	size = 0;
}

void move_array::push_back(int move)
{
	if (size < 7) {
		data[size] = move;
		size++;
	}
	else
		std::cout << "WARNING! move_array allocation out of bounds!" << std::endl;
}

void move_array::clear()
{
	data = clear_arr;
	size = 0;
}

const std::array<int, 7>& move_array::get_data()
{
	return data;
}

unsigned move_array::get_size()
{
	return size;
}

int& move_array::back()
{
	return data[size - 1];
}

int& move_array::begin()
{
	return data[0];
}

bool move_array::empty()
{
	return (data[0] == -1);
}
