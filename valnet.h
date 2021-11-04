#pragma once
#include "state.h"
#include <torch/torch.h>

namespace F = torch::nn::functional;

struct valnetImpl : torch::nn::Module
{
	valnetImpl();
	torch::Tensor forward(torch::Tensor x);
	torch::Tensor forward(const state& s);
<<<<<<< HEAD
	torch::nn::Linear fc1{ nullptr }, fc2{ nullptr }, fc3{ nullptr };
=======
	torch::nn::Conv2d c1, c2;
	torch::nn::Flatten flatten;
	torch::nn::Linear fc1, fc2, fc3;
	torch::Device device;
>>>>>>> polnet
};
TORCH_MODULE(valnet);

