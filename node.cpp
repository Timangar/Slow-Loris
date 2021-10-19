#include "node.h"
#include <cassert>

node& node::operator=(const node& other)
{
	_action = other._action;
	_size = other._size;
	_current = other._current;
	_expanded = other._expanded;
	_history = other._history;
	_n = other._n;
	_t = other._t;
	_o = other._o;

	//_children = other.children();
	return *this;
}

node::node(const node* parent, move action)
	: _n(0), _t(0), _o(0), _action(action), _current(parent->_current)
{
	if (!action.castle &&
		!parent->_current.position[action.destination].get_color() &&
		!parent->_current.position[action.origin].get_type() == 6)
		_history = parent->_history;
	lmg gen;
	gen.gen(_current, action, _history);
	_size = _current.legal_moves.size();
	//delete gen;
	//std::cout << "node created" << std::endl;
}

node::node(state s) : _n(0), _t(0), _o(0), _action(0, 0), _current(s) { _size = _current.legal_moves.size(); }
node::node() : _n(0), _t(0), _o(0), _action(0, 0), _current() {}

node::~node()
{
	//std::cout << "node is deleted" << std::endl;
}

bool node::inherit(state s)
{
	bool ret = false;
	if (_expanded)
	for (unsigned i = 0; i < _size; i++) {
		if (_children[i].current() == s) {
			ret = inherit(i);
			break;
		}
	}
	return ret;
}

bool node::inherit(unsigned index)
{	
	bool ret = false;
	if (index < _size)
	{
		_action = _children[index]._action;
		_size = _children[index]._size;
		_current = _children[index]._current;
		_expanded = _children[index]._expanded;
		_history = _children[index]._history;
		_n = _children[index]._n;
		_t = _children[index]._t;
		_o = _children[index]._o;

		_children = _children[index].children();

		if (_children)
			ret = true;
	}
	return ret;
}

void node::expand()
{
	std::lock_guard<std::mutex> l(lock);
	if (_children.get() == nullptr && _size > 0) {
		node* intermediate = new node[_size];
		for (unsigned i = 0; i < _size; i++) {
			intermediate[i] = { this, _current.legal_moves[i] };
		}
		_children.reset(intermediate);
		_expanded = true;
	}
}

bool node::expanded() const
{
	return _expanded;
}

node* node::get(int i)
{
	if (i >= _size || _children == nullptr)
		__debugbreak();
	return _children.get() + i;
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

const unsigned node::size() const
{
	if (!_expanded)
		return 0;
	return _size;
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
	std::lock_guard<std::mutex> l(lock);
	_n++;
}

void node::increment_t(double value)
{
	std::lock_guard<std::mutex> l(lock);
	_t += value;
}

void node::increment_o()
{
	std::lock_guard<std::mutex> l(lock);
	_o++;
}

void node::decrement_o()
{
	std::lock_guard<std::mutex> l(lock);
	_o--;
}

int node::color() const
{
	return _current.turn;
}

std::unique_ptr<node[]> node::children()
{
	std::unique_ptr<node[]> p = std::move(_children);
	return p;
}
