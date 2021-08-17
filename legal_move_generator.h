#pragma once
#include "state.h"
typedef class legal_move_generator
{
public:
	legal_move_generator();

	void gen(state& s, move m, bool init = false);
protected:
	void gen_opponent_data(int king_pos, int opponent_color, piece* position, int& en_passant);
	bool sonderkonform(int pin_line, int dest, bool check); //easter egg for all german code stealers
	int get_pin_line(int i_pos); //returns the index of the pin line that pins the piece. -1 if not pinned.

	void king_moves(int i_rank, int i_file, int color, piece* position, bool cq, bool ck);
	void bishop_moves(int i_rank, int i_file, int color, piece* position, bool check);
	void knight_moves(int i_rank, int i_file, int color, piece* position, bool check);
	void rook_moves(int i_rank, int i_file, int color, piece* position, bool check);
	void pawn_moves(int i_rank, int i_file, int color, piece* position, int en_passant, bool check);

	void opponent_king_data(int i_rank, int i_file);
	void opponent_bishop_data(int i_rank, int i_file, int king_pos, piece* position);
	void opponent_knight_data(int i_rank, int i_file, int king_pos);
	void opponent_rook_data(int i_rank, int i_file, int king_pos, piece* position);
	void opponent_pawn_data(int i_rank, int i_file, int opponent_color, int king_pos);

	void clear();

	std::vector<std::vector<int>> x_rays; //connect an enemy sliding piece to the king through pieces. include origin.
	std::vector<std::vector<int>> pin_lines; //x_rays that have been verified to pin a piece
	bool attacked_squares[64]; //all the squares the opponent attacks
	std::vector<std::vector<int>> check_lines; //the check lines connecting a piece to the king, including the piece origin

	std::vector<move> legal_moves;
} lmg;

