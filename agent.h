#pragma once
#include "chess_environment.h"
#include "node.h"

class agent
{
public:
	agent(std::string fen = start_fen, double c = 1.5);

	move act(state s);									//act on thoughts
	void train();								//train based on training data

	static std::string const start_fen;			//starting position as fen string
private:
	node* root;									//root of the tree
	double c;									//exploration variable
	bool thinking;								//controls whether loris should be calculating or resting
	unsigned thinkers;

	std::mutex expansion_lock;

	double UCB1(const node& child, int N);		//calculate confidence of a single node
	unsigned select(node& parent);				//return index of most confident child with respect to its parent
	double expand(node& Node);					//expand a leaf node by giving it children
	double mcts_step(node& Node);				//return evaluation of 1 previously unevaluated node
	void mcts(unsigned long long max_depth);				//repeatedly call mcts_step until stop_thinking is called or a max depth is reached

	double eval(const node& Node);				//evaluate a node
	void policy_predict();						//predict the probability distribution across children of a node

	void think_about(state s);					//start to think continuously about the position
	void stop_thinking();							//stop thinking about the position
	void think_pause_for(int seconds);			//pause the thinking process

	void load_weights();						//load weights for neural networks from file
	void create_vnet();							//create hyperparameters for value network
	void create_pnet();							//create hyperparameters for policy network
};
