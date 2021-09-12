#pragma once
#include "chess_environment.h"

class agent
{
public:
	agent();

	move act(state s);
private:
	lmg gen;
};
