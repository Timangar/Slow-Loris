#pragma once
#include "state.h"
#include <torch/torch.h>

namespace F = torch::nn::functional;

struct valnetImpl : torch::nn::Module
{
	valnetImpl();
	torch::Tensor forward(torch::Tensor x);
	torch::Tensor forward(const state& s);
	torch::nn::Linear fc1{ nullptr }, fc2{ nullptr }, fc3{ nullptr };
};
TORCH_MODULE(valnet);

