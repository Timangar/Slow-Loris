#include "perft.h"
#include "play.h"
#include <torch/torch.h>


int main(void)
{
	torch::Tensor t = torch::rand({ 2, 2 });
	std::cout << t << std::endl;
	play();
}