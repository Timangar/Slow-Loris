#pragma once
#include "state.h"
#include "move_discriminator.h"

struct polnetImpl : torch::nn::Module
{
	polnetImpl();
	torch::Tensor forward(const state& s);
	torch::nn::Conv2d c1, c2;
	torch::nn::Flatten flatten;
	torch::nn::Linear fc1, fc2, fc3;
	torch::Device device;

	bool collect;
	std::vector<torch::Tensor> train;
	mdis disc;
};
TORCH_MODULE(polnet);
