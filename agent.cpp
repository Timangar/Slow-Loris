#include "agent.h"
#include <math.h>
#include <thread>
#include <random>
#include <Windows.h>

std::string const agent::start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

agent::agent(std::string fen, double c) : root(state(fen)), c(c), thinking(false), thinkers(0) {}


void agent::think_about(state s)
{
    if (thinking)
        return;

    //construct new root
    bool child_state = false;
    for (node n : root.children()) {
        if (n.current() == s) {
            root = n;
            child_state = true;
            break;
        }
    }

    if (!child_state)
        root = node(s);
    
    //determine max depth and number of threads based on computer stats
    unsigned n_threads = 1;

    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);
    GlobalMemoryStatusEx(&memory_status);

    unsigned long long max_depth = memory_status.ullAvailPhys / 8 / sizeof(node) / n_threads;

    //start threads here
    thinking = true;

    for (unsigned i = 0; i < n_threads; i++) {
        std::thread worker(&agent::mcts, this, max_depth);
        worker.detach();
    }
}

void agent::stop_thinking()
{
    if (!thinking)
        return;

    thinking = false;
    while (thinkers)
        continue;

    return;
}

void agent::think_pause_for(int seconds)
{
}

move agent::act()
{
    if (thinking)
        stop_thinking();

    unsigned index = 0;
    int highscore = 0;
    for (unsigned i = 0; i < root.children().size(); i++) {
        int score = root.children()[i].n();
        if (highscore < score) {
            highscore = score;
            index = i;
        }
    }
    return root.children()[index].action();
}

void agent::train()
{
}

double agent::UCB1(const node& child, int N)
{
	if (child.n())
		return (double)child.t() / ((double)child.n() + (double)child.o()) - c * sqrt(log(N) / ((double)child.n() + (double)child.o()));
	else
		return (double)INFINITY;
}

unsigned agent::select(node& parent)
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
    //the evaluation has to be flipped. a black node with white winning needs val -x, with black winning x and vice versa.
    Node.increment_t(-evaluation * Node.color()); 
    Node.increment_n();
    Node.decrement_o();
    return evaluation;
}

void agent::mcts(unsigned long long max_depth)
{
    thinkers++;
    while (max_depth > 0 && thinking) {
        mcts_step(root);
        max_depth--;
    }
    thinkers--;
}

double agent::eval(const node& Node) //return a positive value if white is winning, a negative value if black is winning
{
    if (Node.terminal())
        return (double)Node.score() * 10000.0;

    //define piece values
    int values[7] = { 0, 0, 9, 3, 3, 5, 1 };
    
    int eval = 0;
    for (piece p : Node.position())
        eval += values[p.get_type()] * p.get_color();
    return eval;
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
