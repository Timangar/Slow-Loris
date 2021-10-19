#include "legal_move_generator.h"
#include <iostream>
#include <cassert>
#include "tools.h"

legal_move_generator::legal_move_generator() : attacked_squares{ 0 } {}

void legal_move_generator::gen(state& s, move m, const std::vector<state>& history, bool init)
{
	if (!init) {
		init = true;
		bool increment_fifty_move_count = true;
		//make normal part of move
		//-------------------------
		//check if move is a captures
		if (s.position[m.destination].get_color())
			increment_fifty_move_count = false;
		//check for promotion, because the piece type has to be changed
		if (m.promotion)
			s.position[m.destination] = {2, s.turn};
		else
			s.position[m.destination] = s.position[m.origin];
		s.position[m.origin] = piece();

		//check whether an en passant capture is possible on the next move
		//-----------------------------------------------------------------
		s.en_passant = 0;
		if (s.position[m.destination].get_type() == 6)
		{
			if (s.position[m.destination].get_color() == WHITE && floor(m.destination / 8) == 4 && floor(m.origin / 8) == 6)
				s.en_passant = m.destination + 8;
			else if (s.position[m.destination].get_color() == BLACK && floor(m.destination / 8) == 3 && floor(m.origin / 8) == 1)
				s.en_passant = m.destination - 8;
			increment_fifty_move_count = false;
		}
		
		//check for terminal state due to 50 move rule or threefold repetition
		//----------------------------------------------------------------------
		//50 move rule
		if (increment_fifty_move_count) {
			s.fifty_move_count++;
			if (s.fifty_move_count >= 100) {
				s.terminal_state = true;
				s.score = 0;
			}
		}
		else
			s.fifty_move_count = 0;
		//threefold repetition
		if (history.size() > 4) {
			for (int i = 4; i <= s.fifty_move_count; i += 2) {
				if (history[(history.size() - i)] == s)
					s.repetition_count = history[history.size() - 1].repetition_count + 1;
			}
			if (s.repetition_count >= 4) {
				s.terminal_state = true;
				s.score = 0;
			}
		}

		//check whether the move affects castling rights
		//-----------------------------------------------
		//king move:
		if (s.position[m.destination].get_type() == 1)
		{
			if (s.turn == BLACK) {
				s.castling_b_k = false;
				s.castling_b_q = false;
				s.pos_bk = m.destination;
			}
			else if (s.turn == WHITE) {
				s.castling_w_k = false;
				s.castling_w_q = false;
				s.pos_wk = m.destination;
			}
		}
		//rook move:
		if (s.position[m.destination].get_type() == 5)
		{
			switch (m.origin) {
			case 0:
				s.castling_b_q = false;
				break;
			case 7:
				s.castling_b_k = false;
				break;
			case 56:
				s.castling_w_q = false;
				break;
			case 63:
				s.castling_w_k = false;
				break;
			default:
				break;
			}
		}
		//rook getting captured:
		switch (m.destination) {
		case 0:
			s.castling_b_q = false;
			break;
		case 7:
			s.castling_b_k = false;
			break;
		case 56:
			s.castling_w_q = false;
			break;
		case 63:
			s.castling_w_k = false;
			break;
		default:
			break;
		}

		//execution of special moves
		//--------------------------
		//castling
		if (m.castle) {
			switch (m.destination) {
			case 2:
				s.position[3] = s.position[0];
				s.position[0] = piece();
				break;
			case 6:
				s.position[5] = s.position[7];
				s.position[7] = piece();
				break;
			case 58:
				s.position[59] = s.position[56];
				s.position[56] = piece();
				break;
			case 62:
				s.position[61] = s.position[63];
				s.position[63] = piece();
				break;
			default:
				break;
			}
		}

		//handle if the last move was an en passant capture
		if (m.en_passant) {
			if (s.turn == WHITE)
				s.position[m.destination + 8] = piece();
			else if (s.turn == BLACK)
				s.position[m.destination - 8] = piece();
		}

		s.turn = s.turn * -1;
	}
	//create a map of all the squares attacked by enemy pieces
	//create a map per x-ray to the king to determine pins
	//determine whether the king is in check
	bool check = false;
	bool ck, cq; //castling rights for the current king
	if (s.turn == WHITE) {
		gen_opponent_data(s.pos_wk, BLACK, s.position, s.en_passant);
		if (attacked_squares[s.pos_wk])
			check = true;
		ck = s.castling_w_k;
		cq = s.castling_w_q;
	}
	else {
		gen_opponent_data(s.pos_bk, WHITE, s.position, s.en_passant);
		if (attacked_squares[s.pos_bk])
			check = true;
		ck = s.castling_b_k;
		cq = s.castling_b_q;
	}

	//generate legal moves, considering opponent data
	//get each piece
	for (unsigned int i = 0; i < 64; i++)
	{
		if (legal_moves.size() > 100) {
			tools tool;
			//tool.log_state(s);
		}
		//get every allied piece
		piece opponent;
		if (s.position[i].get_color() == s.turn)
			opponent = s.position[i];
		else
			continue;

		int i_rank = (int)floor(i / 8);
		int i_file = i - i_rank * 8;

		//treat every piece differently
		switch (opponent.get_type())
		{
		case 1:
			king_moves(i_rank, i_file, s.turn, s.position, ck, cq, check);
			break;
		case 2:
			bishop_moves(i_rank, i_file, s.turn, s.position, check);
			rook_moves(i_rank, i_file, s.turn, s.position, check);
			break;
		case 3:
			bishop_moves(i_rank, i_file, s.turn, s.position, check);
			break;
		case 4:
			knight_moves(i_rank, i_file, s.turn, s.position, check);
			break;
		case 5:
			rook_moves(i_rank, i_file, s.turn, s.position, check);
			break;
		case 6:
			pawn_moves(i_rank, i_file, s.turn, s.position, s.en_passant, check);
			break;
		default:
			std::cerr << "ERROR::LEGAL_MOVE_GENERATOR::GENERATOR ERROR::UNKNOWN PIECE WAS PASSED";
		}
	}
	//check for terminal state
	if (!legal_moves.size()) {
		s.terminal_state = true;
		if (check)
			s.score = s.turn * -1;
		else
			s.score = 0;
	}
	s.legal_moves = legal_moves;
	clear();
}

void legal_move_generator::gen_opponent_data(int king_pos, int opponent_color, const std::array<piece, 64>& position, int& en_passant)
{
	//data for king
	//for each piece of opponent color, get their x_ray, their attacked squares and their check line if there
	for (unsigned int i = 0; i < 64; i++)
	{
		//get every piece of opponent color
		piece opponent;
		if (position[i].get_color() == opponent_color)
			opponent = position[i];
		else
			continue;

		int i_rank = (int)floor(i / 8);
		int i_file = i - i_rank * 8;

		//treat every piece differently
		switch (opponent.get_type())
		{
		case 1:
			opponent_king_data(i_rank, i_file);
			break;
		case 2:
			opponent_bishop_data(i_rank, i_file, king_pos, position);
			opponent_rook_data(i_rank, i_file, king_pos, position);
			break;
		case 3:
			opponent_bishop_data(i_rank, i_file, king_pos, position);
			break;
		case 4:
			opponent_knight_data(i_rank, i_file, king_pos);
			break;
		case 5:
			opponent_rook_data(i_rank, i_file, king_pos, position);
			break;
		case 6:
			opponent_pawn_data(i_rank, i_file, opponent_color, king_pos);
			break;
		default:
			std::cerr << "ERROR::LEGAL_MOVE_GENERATOR::GENERATOR ERROR::UNKNOWN PIECE WAS PASSED";
		}
	}

	//generate pin_lines from x_rays
	//-------------------------------
	//normal case
	for (move_array v : x_rays) {
		if (v.empty())
			continue;
		int obst_count = -1;
		for (unsigned j = 0; j < v.get_size(); j++)
			if (position[v.get_data()[j]].get_color())
				obst_count++;
		if (obst_count == 1)
			for (int i = 0; i < pin_lines.size(); i++)
				if (pin_lines[i].empty()) {
					pin_lines[i] = v;
					break;
				}
	}
	//add case for en passant pin
	if (en_passant) {
		int ep_rank;
		if (opponent_color == WHITE)
			ep_rank = 4;
		else
			ep_rank = 3;
		/*this can only happen if the king is on the rank where two pieces on the same rank can be removed with
		one move. only the en passant ranks.*/ 
		if (floor(king_pos / 8) == ep_rank) {
			for (move_array v : x_rays) {
				if (v.empty())
					continue;
				int obst_count = -1;		//there have to be exactly two pieces
				int pawn_count = 0;			//both pieces have to be pawns
				int same_color_count = -1;	//exactly one of them has to have the same color as the x_ray piece
				if (floor(v.back() / 8) != ep_rank)
					continue;
				for (unsigned j = 0; j < v.get_size(); j++) {
					if (position[v.get_data()[j]].get_color()) {
						obst_count++;
						if (position[v.get_data()[j]].get_type() == 6)
							pawn_count++;
						if (position[v.get_data()[j]].get_color() == opponent_color)
							same_color_count++;
					}
				}
					
				if (obst_count == 2 && pawn_count == 2 && same_color_count == 1)
					en_passant = false;
			}
		}
	}

}

bool legal_move_generator::sonderkonform(int pin_line, int dest, bool check)
{
	bool check_conform = true;
	bool pin_conform = true;

	//handle check
	if (check) {
		check_conform = false;
		assert(check_lines[1].empty());
		for (int j : check_lines[0].get_data())
			if (dest == j) {
				check_conform = true;
				break;
			}
	}

	//handle pins
	if (pin_line >= 0) { //there is a pin on this piece if evaluated as true
		pin_conform = false;
		for (int j : pin_lines[pin_line].get_data())
			if (dest == j) {
				pin_conform = true;
				break;
			}
	}
	
	if (check_conform && pin_conform)
		return true;
	else
		return false;
}

int legal_move_generator::get_pin_line(int i_pos)
{
	for (int pinline = 0; pinline < pin_lines.size(); pinline++)
		for (int j : pin_lines[pinline].get_data())
			if (i_pos == j)
				return pinline;

	return -1;
}

void legal_move_generator::king_moves(int i_rank, int i_file, int color,
	const std::array<piece, 64>& pos, bool ck, bool cq, bool check)
{
	int i = i_rank * RANK + i_file;
	if (i_rank && !attacked_squares[i - RANK] && pos[i - RANK].get_color() != color)
		legal_moves.push_back({ i, i - RANK });
	if (i_file && !attacked_squares[i - FILE__] && pos[i - FILE__].get_color() != color)
		legal_moves.push_back({ i, i - FILE__ });
	if (i_rank < 7 && !attacked_squares[i + RANK] && pos[i + RANK].get_color() != color)
		legal_moves.push_back({ i, i + RANK });
	if (i_file < 7 && !attacked_squares[i + FILE__] && pos[i + FILE__].get_color() != color)
		legal_moves.push_back({ i, i + FILE__ });
	if (i_file && i_rank && !attacked_squares[i - FILE__ - RANK] && pos[i - FILE__ - RANK].get_color() != color)
		legal_moves.push_back({ i, i - FILE__ - RANK });
	if (i_file && i_rank < 7 && !attacked_squares[i - FILE__ + RANK] && pos[i - FILE__ + RANK].get_color() != color)
		legal_moves.push_back({ i, i - FILE__ + RANK });
	if (i_file < 7 && i_rank < 7 && !attacked_squares[i + FILE__ + RANK] && pos[i + FILE__ + RANK].get_color() != color)
		legal_moves.push_back({ i, i + FILE__ + RANK });
	if (i_file < 7 && i_rank && !attacked_squares[i + FILE__ - RANK] && pos[i + FILE__ - RANK].get_color() != color)
		legal_moves.push_back({ i, i + FILE__ - RANK });

	//castling rights
	int rank = 0;
	if (color == WHITE)
		rank = 7;
	if (!check) 
	{
		if (ck)
		{
			if (!attacked_squares[rank * RANK + 5])
				if (!attacked_squares[rank * RANK + 6])
					if (!pos[rank * RANK + 5].get_color())
						if (!pos[rank * RANK + 6].get_color())
							legal_moves.push_back({ i, rank * RANK + 6, true });
		}
		if (cq)
		{
			if (!attacked_squares[rank * RANK + 2] && !attacked_squares[rank * RANK + 3]
				&& !pos[rank * RANK + 2].get_color() && !pos[rank * RANK + 3].get_color())
				legal_moves.push_back({ i, rank * RANK + 2, true });
		}
	}
}

void legal_move_generator::bishop_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, bool check)
{
	//if double check, return. only king can move
	if (check) {
		if (!check_lines[1].empty())
			return;
	}

	//initialize i_pos
	int i = i_rank * RANK + i_file;

	//get pinline
	int pinline = get_pin_line(i);

	//NW direction
	for (int rank = i_rank - 1, file = i_file - 1;
		rank >= 0 && file >= 0; rank--, file--)
	{
		int dest = rank * RANK + file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}

	//NE direction
	for (int rank = i_rank - 1, file = i_file + 1;
		rank >= 0 && file <= 7; rank--, file++)
	{
		int dest = rank * RANK + file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}

	//SE direction
	for (int rank = i_rank + 1, file = i_file + 1;
		rank <= 7 && file <= 7; rank++, file++)
	{
		int dest = rank * RANK + file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}

	//SW direction
	for (int rank = i_rank + 1, file = i_file - 1;
		rank <= 7 && file >= 0; rank++, file--)
	{
		int dest = rank * RANK + file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}
}

void legal_move_generator::knight_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, bool check)
{
	//if double check, return. only king can move
	if (check) {
		if (!check_lines[1].empty())
			return;
	}

	int i = i_rank * RANK + i_file;
	int pinline = get_pin_line(i);
	const int NNE = i + 1 - 2 * RANK;
	const int NEE = i - RANK + 2;
	const int SEE = i + RANK + 2;
	const int SSE = i + 2 * RANK + FILE__;
	const int SSW = i - FILE__ + 2 * RANK;
	const int SWW = i - 2 + RANK;
	const int NWW = i - 2 - RANK;
	const int NNW = i - FILE__ - 2 * RANK;

	//north north east
	if (sonderkonform(pinline, NNE, check))
		if (i_file < 7 && i_rank > 1 && position[NNE].get_color() != color)
			legal_moves.push_back({ i, NNE });

	//north east east
	if (sonderkonform(pinline, NEE, check))
		if (i_rank && i_file < 6 && position[NEE].get_color() != color)
			legal_moves.push_back({ i, NEE });

	//south east east
	if (sonderkonform(pinline, SEE, check))
		if (i_rank < 7 && i_file < 6 && position[SEE].get_color() != color)
			legal_moves.push_back({ i, SEE });
	
	//south south east
	if (sonderkonform(pinline, SSE, check))
		if (i_file < 7 && i_rank < 6 && position[SSE].get_color() != color) 
			legal_moves.push_back({ i, SSE });
	
	//south south west
	if (sonderkonform(pinline, SSW, check))
		if (i_file && i_rank < 6 && position[SSW].get_color() != color) 
			legal_moves.push_back({ i, SSW });
	
	// south west west
	if (sonderkonform(pinline, SWW, check))
		if (i_file > 1 && i_rank < 7 && position[SWW].get_color() != color) 
			legal_moves.push_back({ i, SWW });
	
	//north west west
	if (sonderkonform(pinline, NWW, check))
		if (i_file > 1 && i_rank && position[NWW].get_color() != color) 
			legal_moves.push_back({ i, NWW });
	
	//north north west
	if (sonderkonform(pinline, NNW, check))
		if (i_file && i_rank > 1 && position[NNW].get_color() != color) 
			legal_moves.push_back({ i, NNW });
}

void legal_move_generator::rook_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, bool check)
{
	//if double check, return. only king can move
	if (check) {
		if (!check_lines[1].empty())
			return;
	}

	int i = i_rank * RANK + i_file;
	int pinline = get_pin_line(i);

	//up direction
	for (int rank = i_rank - 1; rank >= 0; rank--)
	{
		int dest = rank * RANK + i_file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}

	//down direction
	for (int rank = i_rank + 1; rank <= 7; rank++)
	{
		int dest = rank * RANK + i_file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}

	//left direction
	for (int file = i_file - 1; file >= 0; file--)
	{
		int dest = i_rank * RANK + file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}

	//right direction
	for (int file = i_file + 1; file <= 7; file++)
	{
		int dest = i_rank * RANK + file;
		if (position[dest].get_color() == color)
			break;
		if (sonderkonform(pinline, dest, check))
			legal_moves.push_back({ i, dest });
		if (position[dest].get_color() == color * -1)
			break;
	}
}

void legal_move_generator::pawn_moves(int i_rank, int i_file, int color, const std::array<piece, 64>& position, int en_passant, bool check)
{
	//if double check, return. only king can move
	if (check) {
		if (!check_lines[1].empty())
			return;
	}

	int i = i_rank * 8 + i_file;
	int pinline = get_pin_line(i);
	int promotion_rank = 0;
	bool promotion = false;
	int start_rank = 6;
	int direction = -1;

	assert(color);
	if (color == BLACK) {
		promotion_rank = 7;
		start_rank = 1;
		direction = 1;
	}

	if (i_rank + direction == promotion_rank)
		promotion = true;

	//advance forward one
	int dest = i + direction * RANK;
	if (sonderkonform(pinline, dest, check))
		if (!position[dest].get_color() && dest >= 0)
			legal_moves.push_back({ i, dest, false, false, promotion });

	//advance forward 2
	if (i_rank == start_rank) {
		dest = i + 2 * direction * RANK;
		if (sonderkonform(pinline, dest, check))
			if (!position[dest].get_color() && !position[i + direction * RANK].get_color()) //can't jump over pieces
				legal_moves.push_back({ i, dest });
	}
	//capture left
	if (i_file) {
		dest = i + direction * RANK - 1;
		if (sonderkonform(pinline, dest, check))
		{
			if (position[dest].get_color() == -1 * color)
				legal_moves.push_back({ i, dest, false, false, promotion });
			//en passant
			else if (dest == en_passant && i_rank != start_rank && en_passant)
				legal_moves.push_back({ i, dest, false, true });
		}
	}
	//capture right
	if (i_file < 7) {
		dest = i + direction * RANK + 1;
		if (sonderkonform(pinline, dest, check))
		{ 
			if (position[dest].get_color() == -1 * color)
				legal_moves.push_back({ i, dest, false, false, promotion });
			//en passant
			else if (dest == en_passant && i_rank != start_rank && en_passant)
				legal_moves.push_back({ i, dest, false, true });
		}
	}
}

void legal_move_generator::opponent_king_data(int i_rank, int i_file)
{
	int i = i_rank * RANK + i_file;
	if (i_rank)
		attacked_squares[i - RANK] = true;
	if (i_file)
		attacked_squares[i - FILE__] = true;
	if (i_rank < 7)
		attacked_squares[i + RANK] = true;
	if (i_file < 7)
		attacked_squares[i + FILE__] = true;
	if (i_file && i_rank)
		attacked_squares[i - FILE__ - RANK] = true;
	if (i_file && i_rank < 7)
		attacked_squares[i - FILE__ + RANK] = true;
	if (i_file < 7 && i_rank < 7)
		attacked_squares[i + FILE__ + RANK] = true;
	if (i_file < 7 && i_rank)
		attacked_squares[i + FILE__ - RANK] = true;
}

void legal_move_generator::opponent_rook_data(int i_rank, int i_file, int king_pos, const std::array<piece, 64>& position)
{	
	move_array x_ray;
	move_array check_line;
	bool obst = false;
	//rook - directions are N, S, W and E
			//up direction
	for (int rank = i_rank-1; rank >= 0; rank--)
	{
		unsigned int dest = rank * 8 + i_file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (rank)
					attacked_squares[dest - RANK] = true; //king can't escape backwards
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}

	//clear variables
	x_ray.clear();
	check_line.clear();
	obst = false;

	//down direction
	for (int rank = i_rank+1; rank <= 7; rank++)
	{
		unsigned int dest = rank * 8 + i_file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (rank < 7)
					attacked_squares[dest + RANK] = true; //king can't escape backwards
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}

	//clear variables
	x_ray.clear();
	check_line.clear();
	obst = false;

	//left direction
	for (int file = i_file-1; file >= 0; file--)
	{
		unsigned int dest = i_rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (file)
					attacked_squares[dest - 1] = true; //king can't escape backwards
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}

	//clear variables
	x_ray.clear();
	check_line.clear();
	obst = false;

	//right direction
	for (int file = i_file+1; file <= 7; file++)
	{
		unsigned int dest = i_rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (file < 7)
					attacked_squares[dest + 1] = true;
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}
}

void legal_move_generator::opponent_bishop_data(int i_rank, int i_file, int king_pos, const std::array<piece, 64>& position)
{
	move_array x_ray;
	move_array check_line;
	bool obst = false;
	//bishop - directions are -rank * 8 - file, -rank * 8 + file, rank * 8 - file, rank * 8 + file
	//north east direction
	for (int rank = i_rank - 1, file = i_file + 1; rank >= 0 && file <= 7; rank--, file++)
	{
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (rank && file < 7)
					attacked_squares[dest - RANK + 1] = true;
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}

	//clear variables
	x_ray.clear();
	check_line.clear();
	obst = false;

	//north west direction
	for (int rank = i_rank - 1, file = i_file - 1; rank >= 0 && file >= 0; rank--, file--)
	{
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (rank && file)
					attacked_squares[dest - RANK - 1] = true;
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}

	//clear variables
	x_ray.clear();
	check_line.clear();
	obst = false;

	//south west direction
	for (int rank = i_rank + 1, file = i_file - 1; rank <= 7 && file >= 0; rank++, file--)
	{
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (rank < 7 && file)
					attacked_squares[dest + RANK - 1] = true;
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}

	//clear variables
	x_ray.clear();
	check_line.clear();
	obst = false;

	//south east direction
	for (int rank = i_rank + 1, file = i_file + 1; rank <= 7 && file <= 7; rank++, file++)
	{
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.back() = i_rank * 8 + i_file;
				if (rank < 7 && file < 7)
					attacked_squares[dest + RANK + 1] = true;
				push_check_line(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.back() = i_rank * 8 + i_file;
			push_x_ray(x_ray);
			break;
		}
	}
}

void legal_move_generator::opponent_knight_data(int i_rank, int i_file, int king_pos)
{
	//the potential checkline will consist of only the initial square
	int i = i_rank * RANK + i_file;
	move_array checksquare;
	checksquare.begin() = i;

	//north north east
	if (i_file < 7 && i_rank > 1) {
		attacked_squares[i + FILE__ - 2 * RANK] = true;
		if (king_pos == i + FILE__ - 2 * RANK)
			push_check_line(checksquare);
	}
	//north east east
	if (i_rank && i_file < 6) {
		attacked_squares[i - RANK + 2] = true;
		if (king_pos == i - RANK + 2)
			push_check_line(checksquare);
	}
	//south east east
	if (i_rank < 7 && i_file < 6) {
		attacked_squares[i + RANK + 2] = true;
		if (king_pos == i + RANK + 2)
			push_check_line(checksquare);
	}
	//south south east
	if (i_file < 7 && i_rank < 6) {
		attacked_squares[i + 2 * RANK + FILE__] = true;
		if (king_pos == i + 2 * RANK + FILE__)
			push_check_line(checksquare);
	}
	//south south west
	if (i_file && i_rank < 6) {
		attacked_squares[i - FILE__ + 2 * RANK] = true;
		if (king_pos == i - FILE__ + 2 * RANK)
			push_check_line(checksquare);
	}
	// south west west
	if (i_file > 1 && i_rank < 7) {
		attacked_squares[i - 2 + RANK] = true;
		if (king_pos == i - 2 + RANK)
			push_check_line(checksquare);
	}
	//north west west
	if (i_file > 1 && i_rank) {
		attacked_squares[i - 2 - RANK] = true;
		if (king_pos == i -  2 - RANK)
			push_check_line(checksquare);
	}
	//north north west
	if (i_file && i_rank > 1) {
		attacked_squares[i - FILE__ - 2 * RANK] = true;
		if (king_pos == i - FILE__ - 2 * RANK)
			push_check_line(checksquare);
	}
}

void legal_move_generator::opponent_pawn_data(int i_rank, int i_file, int opponent_color, int king_pos)
{
	//no x-rays possible, only check line of size 1 (the pawn itself) and attack_squares
	int i = i_rank * RANK + i_file;
	if (i_file) {
		attacked_squares[i - opponent_color * RANK - FILE__] = true;
		if (king_pos == i - opponent_color * RANK - FILE__)
			push_check_line({ i });
	}
	if (i_file < 7) {
		attacked_squares[i - opponent_color * RANK + FILE__] = true;
		if (king_pos == i - opponent_color * RANK + FILE__)
			push_check_line(i);
	}
}

void legal_move_generator::clear()
{
	for (move_array& v : pin_lines)
		v.clear();
	for (move_array& v : check_lines)
		v.clear();
	for (move_array& v : x_rays)
		v.clear();
	for (int i = 0; i < 64; i++)
		attacked_squares[i] = false;
	legal_moves.clear();
}

void legal_move_generator::push_x_ray(move_array x)
{
	for (move_array& v : x_rays)
		if (v.empty()) {
			v = x;
			return;
		}
}

void legal_move_generator::push_check_line(move_array c)
{
	for (move_array& v : check_lines)
		if (v.empty()) {
			v = c;
			return;
		}
}
