#pragma once
#include <iostream>
#include <string>
#include <chrono>
class timer
{
public:
	timer(std::string name);
	~timer();
private:
	std::string function_name;
	std::chrono::system_clock::time_point begin;
};

