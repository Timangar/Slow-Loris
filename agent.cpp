#include "agent.h"
#include <random>

agent::agent()
{
}

move agent::act(state s)
{
	if (s.terminal_state)
		return move(0, 0);
	std::default_random_engine randgen;
	std::uniform_int_distribution<int> randdist(0, s.legal_moves.size() - 1);

	int action = randdist(randgen);
	return s.legal_moves.at(action);
}
