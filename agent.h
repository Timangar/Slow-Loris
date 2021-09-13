#pragma once
#include "chess_environment.h"
#include "node.h"

class agent
{
public:
	agent(std::string fen = start_fen);

	void think_start();
	void think_stop();
	void think_pause_for(int seconds);

	move act(state s);
	void train();

	static std::string const start_fen;
private:
	node root;

	double UCB1(node child, int N);
	unsigned select(node parent);
	void expand(node Node);
	double mcts_step(node root);

	double eval(node Node);
	void policy_predict();

	void load_weights();
	void create_vnet();
	void create_pnet();
};
