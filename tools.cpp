#include "tools.h"
#include <iostream>

tools::tools() {}

void tools::log_state(state& s)
{
	for (int i = 0; i < 64; i++) //64 is the length of the position array
	{
		if (i % 8 == 0)
			std::cout << std::endl; 
		char t = s.get_position()[i].get_type();
		if (s.get_position()[i].get_color() == -1)
			t = tolower(t);
		if (t == 0)
			std::cout << (char)254;
		else
			std::cout << t;
	}
}
