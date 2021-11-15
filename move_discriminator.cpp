#include "move_discriminator.h"

move_discriminator::move_discriminator() : disc_size(0)
{
	//for every one of the 64 squares, run the move generation for sliding moves, knights and pawns.
	for (unsigned rank = 0; rank <= 7; rank++)
		for (unsigned file = 0; file <= 7; file++) {
			rook(rank, file);
			bishop(rank, file);
			knight(rank, file);
		}
}

torch::Tensor move_discriminator::discriminate(const torch::Tensor& pnetcalc, const state& s, const torch::Device& device)
{
	//setup return Tensor
	torch::Tensor x = torch::zeros({ (long long)s.legal_moves.size() }, device);

	if (s.turn == 1)
		for (unsigned i = 0; i < s.legal_moves.size(); i++)
			x[i] = pnetcalc[find(s.legal_moves[i])];
	else
		for (unsigned i = 0; i < s.legal_moves.size(); i++)
			x[i] = pnetcalc[inverse_find(s.legal_moves[i])];

	x = torch::nn::functional::softmax(x, 0);
	
	return x;
}

unsigned move_discriminator::find(const move& m)
{
	return finder[m];
}

unsigned move_discriminator::inverse_find(const move& m)
{
	move n((63 - m.origin), (63 - m.destination));
	return finder[n];
}

void move_discriminator::rook(int rank, int file)
{
	int from = 8 * rank + file;

	//up direction
	for (int r = rank - 1; r >= 0; r--) {
		int to = 8 * r + file;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//right direction
	for (int f = file + 1; f <= 7; f++) {
		int to = 8 * rank + f;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//down direction
	for (int r = rank + 1; r <= 7; r++) {
		int to = 8 * r + file;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//left direction
	for (int f = file - 1; f >= 0; f--) {
		int to = 8 * rank + f;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}
}

void move_discriminator::bishop(int rank, int file)
{
	int from = 8 * rank + file;

	//up right direction
	for (int r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++) {
		int to = 8 * r + f;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//down right direction
	for (int r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++) {
		int to = 8 * r + f;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//down left direction
	for (int r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--) {
		int to = 8 * r + f;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//up right direction
	for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
		int to = 8 * r + f;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}
}

void move_discriminator::knight(int rank, int file)
{
	int from = 8 * rank + file;

	//north north east
	if (file < 7 && rank > 1) {
		int to = (rank - 2) * 8 + file + 1;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//north east east
	if (rank && file < 6) {
		int to = (rank - 1) * 8 + file + 2;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//south east east
	if (rank < 7 && file < 6) {
		int to = (rank + 1) * 8 + file + 2;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//south south east
	if (file < 7 && rank < 6) {
		int to = (rank + 2) * 8 + file + 1;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//south south west
	if (file && rank < 6) {
		int to = (rank + 2) * 8 + file - 1;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	// south west west
	if (file > 1 && rank < 7) {
		int to = (rank + 1) * 8 + file - 2;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//north west west
	if (file > 1 && rank) {
		int to = (rank - 1) * 8 + file - 2;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}

	//north north west
	if (file && rank > 1) {
		int to = (rank - 2) * 8 + file - 1;
		finder.insert({ {from, to}, disc_size });
		disc_size++;
	}
}
