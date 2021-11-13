#include "node.h"
#include <cassert>
#include "tools.h"

node& node::operator=(const node& other)
{
	_action = other._action;
	_size = other._size;
	_current = other._current;
	_expanded = other._expanded;
	_history = other._history;
	_move_prob = other._move_prob;
	_n = other._n;
	_t = other._t;
	_o = other._o;

	return *this;
}

node::node(const node* parent, const move& action, double prob)
	: _n(0), _t(0), _o(0), _action(action), _current(parent->_current)
{
	//change color of node
	_current.turn *= -1;
	_move_prob = prob;

	volatile bool pb = true;
	failed_on = 4;

	if (action.castle) {
		pb = false;
		failed_on = 1;
	}
	if (_current.position[action.destination].get_color()) {
		pb = false;
		failed_on = 2;
	}
	if (_current.position[action.origin].get_type() == 6) {
		pb = false;
		failed_on = 3;
	}
	if (pb) {
		_history = parent->_history;
		_history.push_back(_current);
	}
}

node::node(const state& s, std::vector<state> history, const move& m) : _n(0), _t(0), _o(0), _action(m), _current(s), _move_prob(-1.0f)
{ 
	//these are only called when reassigning the root in agent. color of state remains unchanged.
	_history = history;
	_size = _current.legal_moves.size(); 
	failed_on = 0;
}
node::node() : _n(0), _t(0), _o(0), _action(0, 0), _current(), _move_prob(-1.0f) { failed_on = 0; }

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

void node::expand(polnet pn)
{
	std::lock_guard<std::mutex> l(lock);
	torch::NoGradGuard no_grad;
	if (!_expanded) {
		if (!_current.terminal_state && !_size) { //it is possible that the pos is already evaluated if reassignment has happened
			//calculate possible moves
			if (failed_on < 0 || 4 < failed_on) {
				tools tool;
				tool.log_state(_current);
				__debugbreak();
			}
			lmg gen;
			gen.gen(_current, _action, _history);
			_size = _current.legal_moves.size();
		}
		if (_size) {
			//expand tree by amount of legal moves
			node* intermediate = new node[_size];

			//calculate move probabilities
			torch::Tensor probs = pn->forward(_current);
			for (int i = 0; i < _size; i++) {
				double m_prob;
				try {
					m_prob = probs.index({ i }).item<double>();
				}
				catch (const c10::Error e) {
					std::cerr << "ERROR::NODE: RACE CONDITION";
				}
				intermediate[i] = { this, _current.legal_moves[i], m_prob };
			}
			_children.reset(intermediate);
		}
		_expanded = true;
	}
}

bool node::expanded() const
{
	return _expanded;
}

node* node::get(int i)
{
	if (!_expanded)
		__debugbreak();
	return _children.get() + i;
}

int node::n() const
{
	return _n;
}

float node::t() const
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

double node::move_prob() const
{
	return _move_prob;
}

void node::set_move_prob(double n_prob)
{
	_move_prob = n_prob;
}

std::unique_ptr<node[]> node::children()
{
	std::unique_ptr<node[]> p = std::move(_children);
	return p;
}
