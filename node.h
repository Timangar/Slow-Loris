#pragma once
#include "legal_move_generator.h"

class node
{
public:
	node(const node* parent, move action);
	node(state s);

	void expand();

	int n() const;
	int t() const;
	int o() const;

	bool terminal();				//true if the state is a terminal state
	int score();					//1, 0 or -1 (white win, draw or black win)

	void increment_n();
	void increment_t(double value);
	void increment_o();
	void decrement_o();

	int color();

	std::vector<node>& children();

private:
	state current;					//current state of this node
	std::vector<state> _history;	//history leading to this node

	std::vector<node> _children;	//leaf nodes

	int _n;							//visit count
	double _t;						//total value
	int _o;							//active threads on node

	move _action;					//the action that was taken to get to this state
};

