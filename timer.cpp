#include "timer.h"

timer::timer(std::string name)
{
	function_name = name;
	begin = std::chrono::system_clock::now();
}

timer::~timer()
{
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = end - begin;

	std::cout << "The function " << function_name << " took " << diff.count() << " seconds to complete." << std::endl;
}
