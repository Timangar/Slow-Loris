#include "polnet.h"

polnetImpl::polnetImpl() :
	device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU),
	c1(register_module("c1", torch::nn::Conv2d(6, 128, 4))),
	c2(register_module("c2", torch::nn::Conv2d(128, 128, 5))),
	flatten(register_module("flatten", torch::nn::Flatten())),
	fc1(register_module("fc1", torch::nn::Linear(128, 512))),
	fc2(register_module("fc2", torch::nn::Linear(512, 1024))),
	fc3(register_module("fc3", torch::nn::Linear(1024, 1792)))
{}

torch::Tensor polnetImpl::forward(const state & s)
{
    torch::NoGradGuard no_grad;
    //set up the tensor from a given state
    torch::Tensor x = torch::zeros({ 1, 6, 8, 8 }, device).contiguous();

    //run through the position. the piece type converts to the 0d of the tensor,
    //the piece color will be saved. the "i" index is the 1d of the tensor

    //take into account that the tensor must be rotated
    if (s.turn == 1)
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                piece p = s.position[(size_t)i * (size_t)8 + (size_t)j];
                unsigned ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x[0][ptype - 1][i][j] = pcolor;
            }
    else
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                piece p = s.position[(size_t)i * (size_t)8 + (size_t)j];
                unsigned ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x[0][ptype - 1][(int64_t)7 - (int64_t)i][(int64_t)7 - (int64_t)j] = -pcolor;
            }
    try {
    x = torch::relu(c1(x.contiguous()));
    x = flatten(torch::relu(c2(x.contiguous())));
    x = torch::relu(fc1(x.contiguous()));
    x = torch::relu(fc2(x.contiguous()));
    x = fc3(x.contiguous());
    x = torch::softmax(x.contiguous().view(1792), 0);

    torch::Tensor ret = disc.discriminate(x, s, device);

    return ret.detach();
    }
    catch (const c10::Error e) {
        std::cerr << std::endl << "ERROR::POLNET: RACE CONDITION" << std::endl << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

torch::Tensor polnetImpl::forward(torch::Tensor x)
{
    x = torch::relu(c1(x));
    x = flatten(torch::relu(c2(x)));
    x = torch::relu(fc1(x));
    x = torch::relu(fc2(x));
    x = fc3(x);
    x = torch::softmax(x, 1);

    return x;
}
