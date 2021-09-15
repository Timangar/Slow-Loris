#include "legal_move_array.h"
#include <iostream>

legal_move_array::legal_move_array()
	: max_size(127)
{
	_size = 0;
	data.fill({});
}

legal_move_array::legal_move_array(move i)
	: max_size(127)
{
	_size = max_size;
	data.fill(i);
}

legal_move_array& legal_move_array::operator=(const legal_move_array& other)
{
	_size = other._size;
	data = other.data;
	return *this;
}

move legal_move_array::operator[](unsigned i)
{
	return data[i];
}

void legal_move_array::push_back(move m)
{
	if (_size <= max_size) {
		data[_size] = m;
		_size++;
	}
	else
		std::cout << "WARNING! move_array allocation out of bounds!" << std::endl;
}

void legal_move_array::clear()
{
	_size = 0;
	data.fill({});
}

const std::array<move, 128>& legal_move_array::get_data()
{
	// TODO: hier return-Anweisung eingeben
	return data;
}

unsigned legal_move_array::size()
{
	return _size + 1;
}

move& legal_move_array::back()
{
	return data[max_size];
}

move& legal_move_array::begin()
{
	return data[0];
}

bool legal_move_array::empty()
{
	return (!_size);
}
