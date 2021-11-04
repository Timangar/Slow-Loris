#pragma once
#include "chess_environment.h"
#include "node.h"
#include "valnet.h"
<<<<<<< HEAD
=======
#include "polnet.h"
>>>>>>> polnet

class agent
{
public:
<<<<<<< HEAD
	agent(bool load = false, double c = 0.1, std::string fen = start_fen);
	~agent();

	move act(state s);																//act on thoughts
	void train(int target);								//train based on training data
	move train_act(state s, float epsilon);
=======
	agent(bool load = true, double c = 2.5, double learning_rate = 0.0002, std::string fen = start_fen);
	~agent();

	move act(const state& s, const move& m = { 0, 0 });							//act on thoughts
	void train(float target);					//train based on training data
	move train_act(const state& s, std::vector<state> history, const move& m = { 0, 0 });
>>>>>>> polnet

	static std::string const start_fen;			//starting position as fen string
private:
	std::unique_ptr<node> root;					//root of the tree
<<<<<<< HEAD
	double c;									//exploration variable
	valnet vn;									//the value net
	torch::optim::Adam* adam;					//the optimizer
	torch::Device device;						//the device that handles machine learning

	std::vector<torch::Tensor> predictions;
=======
	double c;									//exploration variable - replaced by dynamic polnet eval
	unsigned depth;

	valnet vn;									//the value net
	torch::optim::Adam* val_adam;				//the optimizer for valnet
	polnet pn;
	torch::optim::Adam* pol_adam;
	torch::Device device;						//the device that handles machine learning

	std::mutex dv;								//to savely increment depth

	std::vector<move> played_moves;				//the played moves for training the polnet (targets)
	std::vector<torch::Tensor> positions;		//the played positions for training the neural nets (inputs)

	void dirichlet_noise();						//to ensure exploration, the root is multiplied by dirichlet noise
	void think();								//think about the position
>>>>>>> polnet

	double UCB1(const node* child, int N);		//calculate confidence of a single node
	unsigned select(node* parent);				//return index of most confident child with respect to its parent
	double expand(node* Node);					//expand a leaf node by giving it children
	double mcts_step(node* Node);				//return evaluation of 1 previously unevaluated node
<<<<<<< HEAD
	void mcts(unsigned long long max_depth);	//repeatedly call mcts_step until time is up or max depth is reached

=======
	void mcts(unsigned max_depth);				//repeatedly call mcts_step until time is up or max depth is reached
>>>>>>> polnet
	double eval(const node* Node);				//evaluate a node

<<<<<<< HEAD
	void load_weights();						//load weights for neural networks from file
=======
	torch::Tensor position_convert(const state& s);		//converts an array position into a tensor position
>>>>>>> polnet
};
