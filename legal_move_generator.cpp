#include "legal_move_generator.h"
#include <iostream>
#include <cassert>

legal_move_generator::legal_move_generator() : attacked_squares{ 0 } {}

void legal_move_generator::gen(state& s, move m, bool init)
{
	if (!init) {
		if (m.promotion)
			s.position[m.destination] = {2, s.turn};
		else
			s.position[m.destination] = s.position[m.origin];
		s.position[m.origin] = piece();
		
		if (m.castle) {

		}
		if (m.en_passant) {

		}

		s.turn = s.turn * -1;
	}
	//create a map of all the squares attacked by enemy pieces
	//create a map per x-ray to the king to determine pins
	//determine whether the king is in check
	bool check = false;
	bool ck, cq; //castling rights for the current king
	if (s.turn == WHITE) {
		gen_opponent_data(s.pos_wk, BLACK, s.position);
		if (attacked_squares[s.pos_wk])
			check = true;
		ck = s.castling_w_k;
		cq = s.castling_w_q;
	}
	else {
		gen_opponent_data(s.pos_bk, WHITE, s.position);
		if (attacked_squares[s.pos_bk])
			check = true;
		ck = s.castling_b_k;
		cq = s.castling_b_q;
	}

	//generate legal moves, considering opponent data
	//get each piece
	for (unsigned int i = 0; i < 64; i++)
	{
		//get every allied piece
		piece opponent;
		if (s.position[i].get_color() == s.turn)
			opponent = s.position[i];
		else
			continue;

		int i_rank = floor(i / 8);
		int i_file = i - i_rank * 8;

		//treat every piece differently
		switch (opponent.get_type())
		{
		case 1:
			king_moves(i_rank, i_file, s.turn, s.position, ck, cq);
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
	s.legal_moves = legal_moves;
	clear();
}

void legal_move_generator::gen_opponent_data(int king_pos, int opponent_color, piece* position)
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

		int i_rank = floor(i / 8);
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
}

void legal_move_generator::king_moves(int i_rank, int i_file, int color, piece* pos, bool ck, bool cq)
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
		int rank = 7;
	if (ck)
	{
		if (!attacked_squares[rank * RANK + 5] && !attacked_squares[rank * RANK + 6])
			legal_moves.push_back({ i, rank * RANK + 6, true });
	}
	if (cq)
	{
		if (!attacked_squares[rank * RANK + 2] && !attacked_squares[rank * RANK + 3])
			legal_moves.push_back({ i, rank * RANK + 2, true });
	}
}

void legal_move_generator::bishop_moves(int i_rank, int i_file, int color, piece* position, bool check)
{
}

void legal_move_generator::knight_moves(int i_rank, int i_file, int color, piece* position, bool check)
{
	int i = i_rank * RANK + i_file;
	const int NNE = i + 1 - 2 * RANK;
	const int NEE = i - RANK + 2;
	const int SEE = i + RANK + 2;
	const int SSE = i + 2 * RANK + FILE__;
	const int SSW = i - FILE__ + 2 * RANK;
	const int SWW = i - 2 + RANK;
	const int NWW = i - 2 - RANK;
	const int NNW = i - FILE__ - 2 * RANK;
	//north north east
	if (i_file < 7 && i_rank > 1 && position[NNE].get_color() != color) {
		legal_moves.push_back({ i, NNE });
	}
	//north east east
	if (i_rank && i_file < 6 && position[NEE].get_color() != color) {
		legal_moves.push_back({ i, NEE });
	}
	//south east east
	if (i_rank < 7 && i_file < 6 && position[SEE].get_color() != color) {
		legal_moves.push_back({ i, SEE });
	}
	//south south east
	if (i_file < 7 && i_rank < 6 && position[SSE].get_color() != color) {
		legal_moves.push_back({ i, SSE });
	}
	//south south west
	if (i_file && i_rank < 6 && position[SSW].get_color() != color) {
		legal_moves.push_back({ i, SSW });
	}
	// south west west
	if (i_file > 1 && i_rank < 7 && position[SWW].get_color() != color) {
		legal_moves.push_back({ i, SWW });
	}
	//north west west
	if (i_file > 1 && i_rank && position[NWW].get_color() != color) {
		legal_moves.push_back({ i, NWW });
	}
	//north north west
	if (i_file && i_rank > 1 && position[NNW].get_color() != color) {
		legal_moves.push_back({ i, NNW });
	}
}

void legal_move_generator::rook_moves(int i_rank, int i_file, int color, piece* position, bool check)
{
}

void legal_move_generator::pawn_moves(int i_rank, int i_file, int color, piece* position, int en_passant, bool check)
{
	int i = i_rank * 8 + i_file;
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
	if (!position[dest].get_color() && dest >= 0)
		legal_moves.push_back({ i, dest, false, false, promotion });
	//advance forward 2
	if (i_rank == start_rank) {
		dest = i + 2 * direction * RANK;
		if (!position[dest].get_color())
			legal_moves.push_back({ i, dest });
	}
	//capture left
	if (i_file) {
		dest = i + direction * RANK - 1;
		if (position[dest].get_color() == -1 * color)
			legal_moves.push_back({ i, dest, false, false, promotion });
	}
	//capture right
	if (i_file < 7 && 0 < i_rank && i_rank < 7)
	dest = i + direction * RANK + 1;
	if (position[dest].get_color() == -1 * color)
		legal_moves.push_back({ i, dest, false, false, promotion });
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

void legal_move_generator::opponent_rook_data(int i_rank, int i_file, int king_pos, piece* position)
{	
	//rook - directions are N, S, W and E
			//up direction
	for (int rank = i_rank-1; rank >= 0; rank--)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = rank * 8 + i_file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}

	//down direction
	for (int rank = i_rank+1; rank <= 7; rank++)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = rank * 8 + i_file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}

	//left direction
	for (int file = i_file-1; file >= 0; file--)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = i_rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}

	//right direction
	for (int file = i_file+1; file <= 7; file++)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = i_rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}
}

void legal_move_generator::opponent_bishop_data(int i_rank, int i_file, int king_pos, piece* position)
{
	//bishop - directions are -rank * 8 - file, -rank * 8 + file, rank * 8 - file, rank * 8 + file
	//north east direction
	for (int rank = i_rank - 1, file = i_file + 1; rank >= 0 && file <= 7; rank--, file++)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}

	//north west direction
	for (int rank = i_rank - 1, file = i_file - 1; rank >= 0 && file >= 0; rank--, file--)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}

	//south west direction
	for (int rank = i_rank + 1, file = i_file - 1; rank <= 7 && file >= 0; rank++, file--)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}

	//south east direction
	for (int rank = i_rank + 1, file = i_file + 1; rank <= 7 && file <= 7; rank++, file++)
	{
		static std::vector<int> x_ray;
		static std::vector<int> check_line;
		static bool obst = false;
		unsigned int dest = rank * 8 + file;
		x_ray.push_back(dest);
		if (!obst) {
			attacked_squares[dest] = true;
			if (position[dest].get_color()) //there is a piece here, do not continue to update attacked squares.
				obst = true;
			//if the line hits the king without obstruction, it is a check line. x_rays are unimportant
			if (king_pos == dest) {
				check_line = x_ray;
				check_line.push_back(i_rank * 8 + i_file);
				check_lines.push_back(check_line);
				break;
			}
		}
		//if the line hits the king through obstruction, it is an x_ray and must be postprocessed for pins.
		if (king_pos == dest) {
			x_ray.push_back(i_rank * 8 + i_file);
			x_rays.push_back(x_ray);
			break;
		}
	}
}

void legal_move_generator::opponent_knight_data(int i_rank, int i_file, int king_pos)
{
	//the potential checkline will consist of only the initial square
	int i = i_rank * RANK + i_file;
	std::vector<int> checksquare = { i };

	//north north east
	if (i_file < 7 && i_rank > 1) {
		attacked_squares[i + FILE__ - 2 * RANK] = true;
		if (king_pos == i + FILE__ - 2 * RANK)
			check_lines.push_back(checksquare);
	}
	//north east east
	if (i_rank && i_file < 6) {
		attacked_squares[i - RANK + 2] = true;
		if (king_pos == i - RANK + 2)
			check_lines.push_back(checksquare);
	}
	//south east east
	if (i_rank < 7 && i_file < 6) {
		attacked_squares[i + RANK + 2] = true;
		if (king_pos == i + RANK + 2)
			check_lines.push_back(checksquare);
	}
	//south south east
	if (i_file < 7 && i_rank < 6) {
		attacked_squares[i + 2 * RANK + FILE__] = true;
		if (king_pos == i + 2 * RANK + FILE__)
			check_lines.push_back(checksquare);
	}
	//south south west
	if (i_file && i_rank < 6) {
		attacked_squares[i - FILE__ + 2 * RANK] = true;
		if (king_pos == i - FILE__ + 2 * RANK)
			check_lines.push_back(checksquare);
	}
	// south west west
	if (i_file > 1 && i_rank < 7) {
		attacked_squares[i - 2 + RANK] = true;
		if (king_pos == i - 2 + RANK)
			check_lines.push_back(checksquare);
	}
	//north west west
	if (i_file > 1 && i_rank) {
		attacked_squares[i + 2 - RANK] = true;
		if (king_pos == i +  2 - RANK)
			check_lines.push_back(checksquare);
	}
	//north west west
	if (i_file && i_rank > 1) {
		attacked_squares[i - FILE__ - 2 * RANK] = true;
		if (king_pos == i - FILE__ - 2 * RANK)
			check_lines.push_back(checksquare);
	}
}

void legal_move_generator::opponent_pawn_data(int i_rank, int i_file, int opponent_color, int king_pos)
{
	//no x-rays possible, only check line of size 1 (the pawn itself) and attack_squares
	int i = i_rank * RANK + i_file;
	if (i_file) {
		attacked_squares[i - opponent_color * RANK - FILE__] = true;
		if (king_pos == i - opponent_color * RANK - FILE__)
			check_lines.push_back({ i });
	}
	if (i_file < 7) {
		attacked_squares[i - opponent_color * RANK + FILE__] = true;
		if (king_pos == i - opponent_color * RANK - FILE__)
			check_lines.push_back({ i });
	}
}

void legal_move_generator::clear()
{
	legal_moves.clear();
	for (int i = 0; i < 64; i++)
		attacked_squares[i] = false;
	x_rays.clear();
	check_lines.clear();
}

