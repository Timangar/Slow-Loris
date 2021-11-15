#pragma once
#include "legal_move_generator.h"
#include "polnet.h"
#include <memory>
#include <mutex>

class node
{
public:
	node& operator=(const node& other);
	node(const node* parent, const move& action, double prob = -1.0);
	node(const state& s, std::vector<state> history, const move& m);
	node();
	~node();

	//node& operator=(const node& other);
	
	bool inherit(state s);				//absorb the branch of one of the child nodes, delete rest of tree, returns true if successful
	bool inherit(unsigned index);		//inherit based on index, returns true if successful

	void expand();

	bool expanded() const;

	node* get(int i);

	int n() const;
	float t() const;
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

	double move_prob() const;
	void set_move_prob(double n_prob);


private:
	std::mutex lock;
	state _current;								//current state of this node
	std::vector<state> _history;				//history leading to this node

	int failed_on;

	double _move_prob;							//likelihood of this move being played according to polnet

	std::unique_ptr<node[]> _children;			//leaf nodes
	std::unique_ptr<node[]> children();

	bool _expanded = false;
	unsigned _size = 0;

	int _n;										//visit count
	float _t;									//total value
	int _o;										//active threads on node

	move _action;								//the action that was taken to get to this state
};