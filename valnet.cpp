#include "valnet.h"


valnetImpl::valnetImpl() :
	fc1(register_module("fc1", torch::nn::Linear(384, 64))),
	fc2(register_module("fc2", torch::nn::Linear(64, 64))),
	fc3(register_module("fc3", torch::nn::Linear(64, 1)))
{}

torch::Tensor valnetImpl::forward(torch::Tensor x)
{
	x = torch::tanh(fc1(x));
	x = torch::tanh(fc2(x));
	x = torch::tanh(fc3(x));

	return x;
}

torch::Tensor valnetImpl::forward(const state& s)
{
    //set up the tensor from a given state
    torch::Tensor x = torch::zeros(384);

    //run through the position. the piece type converts to the 0d of the tensor,
    //the piece color will be saved. the "i" index is the 1d of the tensor
    for (unsigned i = 0; i < 64; i++) {
        piece p = s.position[i];
        int ptype = p.get_type();
        int pcolor = p.get_color();
        if (pcolor)
            x[((unsigned)ptype - 1) * 64 + i] = pcolor;
    }

    x = torch::tanh(fc1(x));
    x = torch::tanh(fc2(x));
    x = torch::tanh(fc3(x));

    return x;
}