#pragma once
#include "perft.h"
#include "play.h"
#include "valnet.h"

torch::Device device = (torch::cuda::is_available()) ? torch::kCUDA : torch::kCPU;

int main(void)
{
	std::cout << device.type();
	//train(100000, 100, 0.401f, 0.999f);
}