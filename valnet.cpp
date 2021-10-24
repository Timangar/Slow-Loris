#include "valnet.h"


valnetImpl::valnetImpl() :
    c1(register_module("c1", torch::nn::Conv2d(6, 8, 4))),
    c2(register_module("c2", torch::nn::Conv2d(8, 16, 5))),
    flatten(register_module("flatten", torch::nn::Flatten())),
	fc1(register_module("fc1", torch::nn::Linear(16, 32))),
	fc2(register_module("fc2", torch::nn::Linear(32, 32))),
	fc3(register_module("fc3", torch::nn::Linear(32, 1)))
{}

torch::Tensor valnetImpl::forward(torch::Tensor x)
{
    x = torch::relu(c1(x));
    x = torch::relu(c2(x));
    x = torch::relu(fc1(x));
    x = torch::relu(fc2(x));
    x = torch::tanh(fc3(x));

	return x;
}

torch::Tensor valnetImpl::forward(const state& s)
{
    torch::Device device = torch::cuda::is_available() ? torch::kCUDA : torch::kCPU;

    //set up the tensor from a given state
    torch::Tensor x = torch::zeros({ 1, 6, 8, 8 }, device);

    //run through the position. the piece type converts to the 0d of the tensor,
    //the piece color will be saved. the "i" index is the 1d of the tensor

    //take into account that the tensor must be rotated
    if (s.turn == 1)
        for (unsigned i = 0; i < 8; i++)
            for (unsigned j = 0; j < 8; j++) {
                piece p = s.position[i * 8 + j];
                unsigned ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x[0][ptype - 1][i][j] = pcolor;
            }
    else
        for (unsigned i = 0; i < 8; i++)
            for (unsigned j = 0; j < 8; j++) {
                piece p = s.position[i * 8 + j];
                unsigned ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x[0][ptype - 1][7 - i][7 - j] = -pcolor;
            }

    x = torch::relu(c1(x));
    x = flatten(torch::relu(c2(x)));
    x = torch::relu(fc1(x));
    x = torch::relu(fc2(x));
    x = torch::tanh(fc3(x));

    return x * s.turn;
}