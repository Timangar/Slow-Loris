#include "valnet.h"


valnetImpl::valnetImpl() :
    //device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU),
    device(torch::kCPU),
    c1(register_module("c1", torch::nn::Conv2d(6, 128, 4))),
    c2(register_module("c2", torch::nn::Conv2d(128, 128, 5))),
    flatten(register_module("flatten", torch::nn::Flatten())),
	fc1(register_module("fc1", torch::nn::Linear(128, 128))),
	fc2(register_module("fc2", torch::nn::Linear(128, 128))),
	fc3(register_module("fc3", torch::nn::Linear(128, 1)))
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

double valnetImpl::forward(const state& s)
{
    torch::NoGradGuard no_grad;
    //set up the tensor from a given state
    torch::Tensor x = torch::zeros({ 1, 6, 8, 8 }, torch::kCPU).contiguous();

    //run through the position. the piece type converts to the 0d of the tensor,
    //the piece color will be saved. the "i" index is the 1d of the tensor

    //take into account that the tensor must be rotated
    if (s.turn == 1)
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                piece p = s.position[(size_t)i * (size_t)8 + (size_t)j];
                int ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x.index_put_({ 0, ptype - 1, i, j }, pcolor);
            }
    else
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                piece p = s.position[(size_t)i * (size_t)8 + (size_t)j];
                int ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x.index_put_({ 0, ptype - 1, 7 - i, 7 - j }, -pcolor);
            }

    //copy to used device
    x = x.to(device);

    try {
        x = torch::relu(c1(x));
        x = flatten(torch::relu(c2(x)));
        x = torch::relu(fc1(x));
        x = torch::relu(fc2(x));
        x = torch::tanh(fc3(x));

        x = x.detach();

    return x.item<double>();
    }
    catch (const c10::Error e) {
        std::cerr << std::endl << "ERROR::VALNET: RACE CONDITION" << std::endl;
        std::cerr << e.what_without_backtrace() << std::endl << std::endl;
    }
}