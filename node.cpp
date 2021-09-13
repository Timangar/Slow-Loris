#include "node.h"

node::node(const node* parent, move action)
	: n(0), t(0), o(0), action(action), current(parent->current)
{
	if (!action.castle &&
		!parent->current.position[action.destination].get_color() &&
		!parent->current.position[action.origin].get_type() == 6)
		history = parent->history;
	lmg gen;
	gen.gen(current, action, history);
}
