#pragma once
#include "state.h"
#include "move_discriminator.h"

struct polnetImpl : torch::nn::Module
{
	polnetImpl();
	torch::Tensor forward(const state& s);		//for inference
	torch::Tensor forward(torch::Tensor x); //for recording the full tensor for training
	torch::nn::Conv2d c1, c2;
	torch::nn::Flatten flatten;
	torch::nn::Linear fc1, fc2, fc3;
	torch::Device device;

	mdis disc;
};
TORCH_MODULE(polnet);
