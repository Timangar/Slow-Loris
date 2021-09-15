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
	if (en_passant_fen[0] != '-')
		en_passant = en_passant_fen[0] - 'a' + 8 * en_passant_rank;
	else
		en_passant = 0;	//zero, although a square on the board, can't be an en passant square and will therefore be used as '-'

	//initialize the repetition count with zero (no information is given)
	repetition_count = 0;

	//initialize the fifty move count
	fifty_move_count = std::stoi(fen_components[4]);

	//initialize terminal state with false, why would you load a finished game?
	terminal_state = false;
	score = 0;
}

bool state::contains(move& m)
{
	for (unsigned i = 0; i < legal_moves.size(); i++)
		if (legal_moves[i] == m)
			return true;
	return false;
}

std::array<piece, 64>* state::get_position()
{
	return &position;
}

int state::get_turn()
{
	return turn;
}

bool state::operator==(const state& other) const
{
	if (
		position == other.position &&
		en_passant == other.en_passant &&
		*this % other
		)
		return true;
	return false;
}

bool state::operator%(const state& other) const
{
	if (
		castling_b_k == other.castling_b_k &&
		castling_b_q == other.castling_b_q &&
		castling_w_k == other.castling_w_k &&
		castling_w_q == other.castling_w_q
		)
		return true;
	return false;
}
