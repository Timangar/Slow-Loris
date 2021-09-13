#include "node.h"
#include <cassert>

node::node(const node* parent, move action)
	: _n(0), _t(0), _o(0), _action(action), current(parent->current)
{
	if (!action.castle &&
		!parent->current.position[action.destination].get_color() &&
		!parent->current.position[action.origin].get_type() == 6)
		_history = parent->_history;
	lmg gen;
	gen.gen(current, action, _history);
	_children.reserve(current.legal_moves.size());
}

node::node(state s) : _n(0), _t(0), _o(0), _action(0, 0), current(s) {}

void node::expand()
{
	for (move m : current.legal_moves)
		_children.emplace_back(this, m);
}

int node::n() const
{
	return _n;
}

int node::t() const
{
	return _t;
}

int node::o() const
{
	return _o;
}

bool node::terminal()
{
	return current.terminal_state;
}

int node::score()
{
	assert(current.terminal_state);
	return current.score;
}

void node::increment_n()
{
	_n++;
}

void node::increment_t(double value)
{
	_t += value;
}

void node::increment_o()
{
	_o++;
}

void node::decrement_o()
{
	_o--;
}

int node::color()
{
	return current.turn;
}

std::vector<node>& node::children()
{
	return _children;
}
