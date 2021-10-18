#include "agent.h"
#include <math.h>
#include <thread>
#include <random>
#include <Windows.h>
#include <mutex>

std::string const agent::start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

agent::agent(std::string fen, double c) : c(c), thinking(false), thinkers(0), root(new node) {}


void agent::think()
{
    if (thinking)
        return;

    //determine max depth and number of threads based on computer stats
    const unsigned n_threads = 4;

    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);
    GlobalMemoryStatusEx(&memory_status);

    unsigned long long max_depth = memory_status.ullAvailPhys / 8 / sizeof(node) / n_threads;
    //start threads here

    std::thread workers[n_threads];

    for (unsigned i = 0; i < n_threads; i++) {
        workers[i] = std::thread(&agent::mcts, this, max_depth);
    }
    for (unsigned i = 0; i < n_threads; i++) {
        workers[i].join();
    }
}

move agent::act(state s)
{
    //check if state is already in calculation
    bool reassign = !root->inherit(s);
    if (reassign) {
        root.reset(new node(s));
    }

    think();

    unsigned index = 0;
    int highscore = 0;
    for (unsigned i = 0; i < root->size(); i++) {
        int score = root->get(i)->n();
        if (highscore < score) {
            highscore = score;
            index = i;
        }
    }
   
    //make root the chosen child
    move action = root->get(index)->action();
    root->inherit(index);

    return action;
}

void agent::train()
{
}

double agent::UCB1(const node* child, int N)
{
	if (child->n())
		return (double)child->t() / ((double)child->n() + (double)child->o()) - c * sqrt(log(N) / ((double)child->n() + (double)child->o()));
	else
		return (double)INFINITY;
}

unsigned agent::select(node* parent)
{
	unsigned index = 0;
	double highscore = double(-INFINITY);
	for (unsigned i = 0; i < parent->size(); i++) {
		double score = UCB1(parent->get(i), parent->n());
		if (highscore < score) {
			highscore = score;
			index = i;
		}
	}
	return index;
}

double agent::expand(node* Node)
{
	//for every legal move, we have to create a new node
	Node->expand();
	return mcts_step(Node->get(select(Node)));
}

double agent::mcts_step(node* Node)
//expand the tree and its evaluation by one new evaluated node. returns eval of an examined child node
//To tell other threads in multithreaded MCTS that this node is already being evaluated, o has to be incremented 
//in the forward pass but decremented in the backprop
{
    Node->increment_o();
    bool expanding = false;
    double evaluation;

    //is this a terminal state?
    if (Node->terminal())
        evaluation = eval(Node);
    //no:
    else
    //is this a leaf node?
    if (!Node->expanded())
    {
        //has it been visited before?
        if (Node->n())
            //yes: expand and evaluate best child
        {
            evaluation = expand(Node); //check if terminal state and node cannot be expanded
        }
        else
            //no: evaluate this node
        {
            evaluation = eval(Node);
        }
    }
    else 
        //no (not a leaf node): pick the best child node to examine
    {
        evaluation = mcts_step(Node->get(select(Node)));
    }

    //backpropagate
    //the evaluation has to be flipped. a black node with white winning needs val -x, with black winning x and vice versa.
    Node->increment_t(-evaluation * Node->color()); 
    Node->increment_n();
    Node->decrement_o();
    return evaluation;
}

void agent::mcts(unsigned long long max_depth)
{
    float thinking_time = 1 + (float)root->size() / 10;
    unsigned long long dep = max_depth;
    auto begin = std::chrono::high_resolution_clock::now();
    thread_local auto end = begin;
    thread_local std::chrono::duration<float> duration = end - begin;

    while (dep > 0 && duration.count() < thinking_time) {
        mcts_step(root.get());
        dep--;
        end = std::chrono::high_resolution_clock::now();
        duration = end - begin;
    }
}

double agent::eval(const node* Node) //return a positive value if white is winning, a negative value if black is winning
{
    if (Node->terminal())
        return (double)Node->score() * 10000.0;

    //define piece values
    int values[7] = { 0, 0, 9, 3, 3, 5, 1 };
    
    int eval = 0;
    for (piece p : Node->position())
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



/*
//is this a leaf node?
    if (!Node.children().size())
        //yes: has it been visited before ?
    {
        if (Node.n())
            //yes: expand and evaluate best child (or
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
*/