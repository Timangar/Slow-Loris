#pragma once
#include "legal_move_generator.h"
#include <memory>

class node
{
public:

	node(const node* parent, move action);
	node(state s);
	node();

	//node& operator=(const node& other);
	
	bool inherit(state s);				//absorb the branch of one of the child nodes, delete rest of tree, returns true if successful
	bool inherit(unsigned index);		//inherit based on index, returns true if successful

	void expand();

	bool expanded() const;

	node* get(int i);

	int n() const;
	int t() const;
	int o() const;

	bool terminal() const;				//true if the state is a terminal state
	int score() const;					//1, 0 or -1 (white win, draw or black win)

	const state& current() const;
	const unsigned size() const;

	std::array<piece, 64> position() const;
	const move& action() const;

	void increment_n();
	void increment_t(double value);
	void increment_o();
	void decrement_o();

	int color() const;


private:
	state _current;								//current state of this node
	std::vector<state> _history;				//history leading to this node

	std::unique_ptr<node[]> _children;			//leaf nodes
	std::unique_ptr<node[]> children();

	bool _expanded = false;
	unsigned _size = 0;

	volatile int _n;							//visit count
	volatile double _t;							//total value
	volatile int _o;							//active threads on node

	move _action;					//the action that was taken to get to this state
};