#include "node.h"
#include <cassert>

node::node(const node* parent, move action)
	: _n(0), _t(0), _o(0), _action(action), _current(parent->_current), _expanded(false)
{
	if (!action.castle &&
		!parent->_current.position[action.destination].get_color() &&
		!parent->_current.position[action.origin].get_type() == 6)
		_history = parent->_history;
	lmg gen;
	gen.gen(_current, action, _history);
	_children.reserve(_current.legal_moves.size());
}

node::node(state s) : _n(0), _t(0), _o(0), _action(0, 0), _current(s), _expanded(false)
{
	_children.reserve(_current.legal_moves.size());
}

void node::expand()
{
	if (!_expanded) {
		std::vector<node> intermediate;
		intermediate.reserve(_current.legal_moves.size());
		for (move m : _current.legal_moves)
			intermediate.emplace_back(this, m);
		_children = intermediate;
		_expanded = true;
	}
}

bool node::expanded() const
{
	return _expanded;
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

bool node::terminal() const
{
	return _current.terminal_state;
}

int node::score() const
{
	assert(_current.terminal_state);
	return _current.score;
}

const state& node::current() const
{
	return _current;
}

std::array<piece, 64> node::position() const
{
	return _current.position;
}

const move& node::action() const
{
	// TODO: hier return-Anweisung eingeben
	return _action;
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

int node::color() const
{
	return _current.turn;
}

std::vector<node>& node::children()
{
	return _children;
}
