#pragma once
#include <array>
#include <iostream>

typedef class move_array
{
public:
	move_array();
	move_array(int i);

	void push_back(int move);
	void clear();

	const std::array<int, 7>& get_data();
	unsigned get_size();

	int& back();
	int& begin();

	bool empty();

private:
	unsigned size;
	std::array<int, 7> data;
	static std::array<int, 7> const clear_arr;
} marr;

