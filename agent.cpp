#include "agent.h"
#include <math.h>
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

double agent::UCB1(const node& child, int N)
{
	if (child.n())
		return child.t() / (child.n() + child.o()) - c * sqrt(log(N) / (child.n() + child.o()));
	else
		return (double)INFINITY;
}

unsigned agent::select(const node& parent)
{
	unsigned index = 0;
	double highscore = double(-INFINITY);
	for (unsigned i = 0; i < parent.children().size(); i++) {
		double score = UCB1(parent.children()[i], parent.n());
		if (highscore < score) {
			highscore = score;
			index = i;
		}
	}
	return index;
}

double agent::expand(node& Node)
{
	//for every legal move, we have to create a new node
	Node.expand();
	return eval(Node.children()[select(Node)]);
}

double agent::mcts_step(node& Node)
//expand the tree and its evaluation by one new evaluated node. returns eval of an examined child node
//To tell other threads in multithreaded MCTS that this node is already being evaluated, o has to be incremented 
//in the forward pass but decremented in the backprop
{
    Node.increment_o();
    int workers_as_entering = Node.o();
    double evaluation;

    //is this a leaf node?
    if (!Node.children().size()) 
        //yes: has it been visited before ?
    {
        if (Node.n())
            //yes: expand and evaluate best child
        {
            if (Node.terminal())                //check if terminal state and node cannot be expanded
                return Node.score();
            else
                evaluation = expand(Node);
        }
        else
            //no: is another thread currently running on this node?
        {
            if (workers_as_entering >= 2)
                //yes: expand and evaluate best child
            {
                if (Node.terminal())                //check if terminal state and node cannot be expanded
                    return Node.score();
                else
                    evaluation = expand(Node);
            }
            else
                //no: evaluate this node
            {
                evaluation = eval(Node);
            }
        }
    }
    else
        //no: pick the best child node to examine
    {
        evaluation = mcts_step(Node.children()[select(Node)]);
    }
    
    //backpropagate
    Node.increment_t(-evaluation * Node.color());
    Node.increment_n();
    Node.decrement_o();
    return evaluation;
}

double agent::eval(const node& Node)
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
