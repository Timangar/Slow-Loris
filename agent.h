#pragma once
#include "chess_environment.h"

class agent
{
public:
	agent();

	void think_start();
	void think_stop();
	void thing_pause_for(int seconds);
	move act(state s);
private:
	lmg gen;
};
