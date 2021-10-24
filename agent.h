#pragma once
#include "chess_environment.h"
#include "node.h"
#include "valnet.h"

class agent
{
public:
	agent(bool load = false, double c = 0.1, std::string fen = start_fen);
	~agent();

	move act(state s);																//act on thoughts
	void train(float target);								//train based on training data
	move train_act(state s, float epsilon);

	static std::string const start_fen;			//starting position as fen string
private:
	std::unique_ptr<node> root;					//root of the tree
	double c;									//exploration variable
	valnet vn;									//the value net
	torch::optim::Adam* adam;					//the optimizer
	torch::Device device;						//the device that handles machine learning
	//std::shared_ptr<valnetImpl> vn;

	std::vector<torch::Tensor> predictions;

	double UCB1(const node* child, int N);		//calculate confidence of a single node
	unsigned select(node* parent);				//return index of most confident child with respect to its parent
	double expand(node* Node);					//expand a leaf node by giving it children
	double mcts_step(node* Node);				//return evaluation of 1 previously unevaluated node
	void mcts(unsigned long long max_depth);	//repeatedly call mcts_step until time is up or max depth is reached

	double eval(const node* Node);				//evaluate a node
	void policy_predict();						//predict the probability distribution across children of a node

	void think();								//think about the position
};
