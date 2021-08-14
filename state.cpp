#include "state.h"
#include <iostream>
#include <sstream>

std::unordered_map<char, int> state::file = {
	{'a', 0},
	{'b', 1},
	{'c', 2},
	{'d', 3},
	{'e', 4},
	{'f', 5},
	{'g', 6},
	{'h', 7},
};

state::state(std::string fen)
{
	position = new piece[64];
	//default initialization so the compiler is happy
	pos_bk = file['e'];
	pos_wk = file['e'] + 56;
	//split fen strings into components
	std::stringstream fen_stream(fen);
	std::vector<std::string> fen_components;
	std::string component;
	while (std::getline(fen_stream, component, ' '))
	{
		fen_components.push_back(component);
	}

	//initialize the board string with the board component from top left to bottom right (like FEN)
	std::string pos_fen = fen_components[0];
	int file = 0;
	int rank = 0;
	for (int i = 0; i < pos_fen.size(); i++)
	{
		char value = pos_fen[i];
		int init_color = 0;

		if (value == '/')
		{
			for (int x = 0; x < (8 - file); x++)
				position[file + x + rank * 8] = piece();
			rank++;
			file = 0;
			continue;
		}
		if (isdigit(value))
		{
			int increment = value - '0';
			for (int x = 0; x < increment; x++)
				position[file + x + rank * 8] = piece();
			file += increment;
			continue;
		}
		if (isupper(value))
			init_color = WHITE;
		else
			init_color = BLACK;
		position[file + rank * 8] = piece((char)toupper(value), init_color);
		if (toupper(value) == 'K')
		{
			if (init_color == WHITE)
				pos_wk = (file + rank * 8);
			else
				pos_bk = (file + rank * 8);
		}
		file++;
	}

	//initialize turn with the second component
	turn = piece::colors[fen_components[1][0]];

	//use the third component to load the castling rights
	std::string castles_fen = fen_components[2];
	castling_b_k = false;
	castling_b_q = false;
	castling_w_k = false;
	castling_w_q = false;
	for (int i = 0; i < castles_fen.size(); i++)
	{
		char value = castles_fen[i];
		switch (value)
		{
		case '-':
			break;
		case 'K':
			castling_w_k = true;
			break;
		case 'k':
			castling_b_k = true;
			break;
		case 'Q':
			castling_w_q = true;
			break;
		case 'q':
			castling_b_q = true;
			break;
		default:
			std::cout << "ERROR::IN FILE STATE.CPP::SOMETHING WEIRD HAS HAPPENED WHILE COMPUTING CASTLING RIGHTS";
		}
	}

	//initialize the index of the en passant piece with the 4. component; 0 if not available (pawns don't reside on a8)
	std::string en_passant_fen = fen_components[3];
	int en_passant_rank;
	if (turn == WHITE)
		en_passant_rank = 2;
	else
		en_passant_rank = 5;
	en_passant = this->file[en_passant_fen[0]] + 8 * en_passant_rank;

	//initialize the repetition count with zero (no information is given)
	repetition_count = 0;

	//initialize the fifty move count
	fifty_move_count = floor(std::stoi(fen_components[4]) / 2);

	//initialize terminal state with false, why would you load a finished game?
	terminal_state = false;
}

state::state(const state& other)
	: legal_moves(other.legal_moves), repetition_count(other.repetition_count),
	fifty_move_count(other.fifty_move_count), terminal_state(other.terminal_state), turn(other.turn),
	pos_bk(other.pos_bk), pos_wk(other.pos_wk), castling_b_k(other.castling_b_k), castling_b_q(other.castling_b_q),
	castling_w_k(other.castling_w_k), castling_w_q(other.castling_w_q), en_passant(other.en_passant)
{
	position = new piece[64];
	memcpy(position, other.position, 64 * sizeof(piece));
}

state::~state()
{
	delete[] position;
}

bool state::contains(move& m)
{
	for (move i : legal_moves)
		if (i == m)
			return true;
	return false;
}

piece* state::get_position()
{
	return position;
}

int state::get_turn()
{
	return turn;
}