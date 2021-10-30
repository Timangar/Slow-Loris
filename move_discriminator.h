#pragma once
#include <map>
#include "state.h"
#include <torch/torch.h>

typedef class move_discriminator
{
public:
	move_discriminator();
	torch::Tensor discriminate(const torch::Tensor& pnetcalc, const state& legal_moves, const torch::Device& device);
	unsigned find(const move& m);
	unsigned inverse_find(const move& m);

private:
	unsigned disc_size;
	std::map<move, unsigned> finder;

	void rook(int rank, int file);
	void bishop(int rank, int file);
	void knight(int rank, int file);
} mdis;

