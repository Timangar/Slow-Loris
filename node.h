#pragma once
#include "legal_move_generator.h"

class node
{
public:
	node(const node* parent, move action);
	node(state s);
private:
	state current;					//current state of this node
	std::vector<state> history;		//history leading to this node

	std::vector<node> children;		//leaf nodes

	int n;							//visit count
	int t;							//total value
	int o;							//active threads on node

	move action;					//the action that was taken to get to this state
};

