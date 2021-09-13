#include "agent.h"
#include <random>

std::string const chess_environment::start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

agent::agent(std::string fen) : root(state(fen)) {}

void agent::think_start()
{
}

void agent::think_stop()
{
}

void agent::think_pause_for(int seconds)
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

void agent::train()
{
}

double agent::UCB1(node child, int N)
{
	return 0.0;
}

unsigned agent::select(node parent)
{
	return 0;
}

void agent::expand(node Node)
{
}

double agent::mcts_step(node root)
{
	return 0.0;
}

double agent::eval(node Node)
{
	return 0.0;
}

void agent::policy_predict()
{
}

void agent::load_weights()
{
}

void agent::create_vnet()
{
}

void agent::create_pnet()
{
}
