#pragma once
#include "state.h"
#include "move_array.h"

typedef class legal_move_generator
{
public:
	legal_move_generator();

	void gen(state& s, move m, bool init = false);
protected:
	void gen_opponent_data(int king_pos, int opponent_color, const std::array<piece, 64>& position, int& en_passant);
	bool sonderkonform(int pin_line, int dest, bool check); //easter egg for all german code stealers
	int get_pin_line(int i_pos); //returns the index of the pin line that pins the piece. -1 if not pinned.

	void king_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, bool cq, bool ck);
	void bishop_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, bool check);
	void knight_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, bool check);
	void rook_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, bool check);
	void pawn_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, int en_passant, bool check);

	void opponent_king_data(int i_rank, int i_file);
	void opponent_bishop_data(int i_rank, int i_file, int king_pos, const std::array<piece, 64>& position);
	void opponent_knight_data(int i_rank, int i_file, int king_pos);
	void opponent_rook_data(int i_rank, int i_file, int king_pos, const std::array<piece, 64>& position);
	void opponent_pawn_data(int i_rank, int i_file, int opponent_color, int king_pos);

	void clear();

	void push_x_ray(move_array x);
	void push_pin_line(move_array pin_line);
	void push_check_line(move_array c);

	std::array<move_array, 48> x_rays;		//x_rays between sliding pieces and king
	std::array<move_array, 8> pin_lines;	//x_rays that have been verified to pin a piece
	std::array<move_array, 2> check_lines;	//the check lines connecting a piece to the king, including the piece origin
	bool attacked_squares[64];						//all the squares the opponent attacks

	std::vector<move> legal_moves;
} lmg;

