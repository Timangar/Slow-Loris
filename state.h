#pragma once
#include <vector>
#include "move.h"
#include <string>
#include "piece.h"
#include <unordered_map>

struct state
{
	state(std::string fen);
	state(const state& other);
	~state();

	bool contains(move& m);
	piece* get_position();
	int get_turn();
	
	piece* position; //an array representing the FEN board in terms of piece classes and "empty" pieces
	std::vector<move> legal_moves;

	int en_passant;

	int repetition_count;
	int fifty_move_count;

	bool terminal_state;
	int turn; //whose turn is it, 1 for white, -1 for black

	bool castling_b_q;
	bool castling_b_k;
	bool castling_w_q;
	bool castling_w_k;

	int pos_wk;
	int pos_bk;

	static std::unordered_map<char, int> file;
};

