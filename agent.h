#pragma once
#include "chess_environment.h"
#include "node.h"
#include "valnet.h"
#include "polnet.h"

class agent
{
public:
	agent(bool load = false, double c = 2, std::string fen = start_fen);
	~agent();

	move act(state s);							//act on thoughts
	void train(float target);					//train based on training data
	move train_act(state s, float epsilon);

	static std::string const start_fen;			//starting position as fen string
private:
	std::unique_ptr<node> root;					//root of the tree
	double c;									//exploration variable - replaced by dynamic polnet eval
	valnet vn;									//the value net
	torch::optim::Adam* val_adam;				//the optimizer for valnet
	polnet pn;
	torch::optim::Adam* pol_adam;
	torch::Device device;						//the device that handles machine learning

	std::vector<move> played_moves;				//the played moves for training the polnet (y input)
	std::vector<torch::Tensor> polnet_training_data;	//the training x input for the polnet
	std::vector<torch::Tensor> predictions;		//the predictions as x input for valnet training

	float UCB1(const node* child, int N);		//calculate confidence of a single node
	unsigned select(node* parent);				//return index of most confident child with respect to its parent
	double expand(node* Node);					//expand a leaf node by giving it children
	double mcts_step(node* Node);				//return evaluation of 1 previously unevaluated node
	void mcts(unsigned long long max_depth);	//repeatedly call mcts_step until time is up or max depth is reached

	double eval(const node* Node);				//evaluate a node

	void think();								//think about the position
};
