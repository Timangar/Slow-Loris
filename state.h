#pragma once
#include <vector>
#include "move.h"
#include <string>
#include "piece.h"
#include <unordered_map>
#include <array>

struct state
{
	state(std::string fen);

	bool contains(move& m);
	std::array<piece, 64>* get_position();
	int get_turn();

	bool operator==(const state& other) const;
	bool operator%(const state& other) const;
	
	std::array<piece, 64> position; //an array representing the FEN board in terms of piece classes and "empty" pieces
	std::vector<move> legal_moves;

	int en_passant;

	int repetition_count;
	int fifty_move_count; //in half moves

	bool terminal_state;
	int score; //1 if white wins, -1 if black wins, 0 for draw
	int turn; //whose turn is it, 1 for white, -1 for black

	bool castling_b_q;
	bool castling_b_k;
	bool castling_w_q;
	bool castling_w_k;

	int pos_wk;
	int pos_bk;

	static std::unordered_map<char, int> file;
};

