#pragma once
#include "perft.h"
#include "play.h"
#include "valnet.h"

int main(void)
{
	state s("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	valnet vn;
	torch::optim::Adam adam(vn.parameters());

	adam.zero_grad();
	torch::Tensor out = vn(s);
	std::cout << out << std::endl;
	torch::Tensor target = torch::ones({ 1 });

	torch::Tensor loss = torch::mse_loss(out, target);
	loss.backward();
	adam.step();
	adam.zero_grad();

	out = vn(s);
	std::cout << out << std::endl;
}