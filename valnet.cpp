#include "valnet.h"


valnetImpl::valnetImpl() :
    device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU),
    c1(register_module("c1", torch::nn::Conv2d(6, 64, 4))),
    c2(register_module("c2", torch::nn::Conv2d(64, 64, 5))),
    flatten(register_module("flatten", torch::nn::Flatten())),
	fc1(register_module("fc1", torch::nn::Linear(64, 32))),
	fc2(register_module("fc2", torch::nn::Linear(32, 32))),
	fc3(register_module("fc3", torch::nn::Linear(32, 1)))
{}

torch::Tensor valnetImpl::forward(torch::Tensor x)
{
    x = torch::relu(c1(x));
    x = flatten(torch::relu(c2(x)));
    x = torch::relu(fc1(x));
    x = torch::relu(fc2(x));
    x = torch::tanh(fc3(x));

	return x;
}

torch::Tensor valnetImpl::forward(const state& s)
{
    //set up the tensor from a given state
    torch::Tensor x = torch::zeros({ 1, 6, 8, 8 }, device).contiguous();

    //run through the position. the piece type converts to the 0d of the tensor,
    //the piece color will be saved. the "i" index is the 1d of the tensor

    //take into account that the tensor must be rotated
    try {
    if (s.turn == 1)
        for (unsigned i = 0; i < 8; i++)
            for (unsigned j = 0; j < 8; j++) {
                piece p = s.position[(int)i * (int)8 + (int)j];
                unsigned ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x[0][ptype - 1][i][j] = pcolor;
            }
    else
        for (unsigned i = 0; i < 8; i++)
            for (unsigned j = 0; j < 8; j++) {
                piece p = s.position[(int)i * (int)8 + (int)j];
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
    }
    catch (const c10::Error e) {
        std::cerr << e.what() << std::endl;
        std::cout << x;
    }

    return x;
}