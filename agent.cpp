#include "agent.h"
#include <math.h>
#include <thread>
#include <random>
#include <Windows.h>

std::string const agent::start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

agent::agent(bool load, double c, double learning_rate, std::string fen)
    : c(c), root(new node), device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU), depth(0) {
    if (load) {
        torch::load(vn, "valnet.pt");
        torch::load(pn, "polnet.pt");
    }
    vn->to(device);
    pn->to(device);
    val_adam = new torch::optim::Adam(vn->parameters(), torch::optim::AdamOptions(learning_rate));
    pol_adam = new torch::optim::Adam(pn->parameters(), torch::optim::AdamOptions(learning_rate));
}

agent::~agent()
{
    delete val_adam;
    delete pol_adam;
}

void agent::think()
{
    //determine max depth and number of threads
    const unsigned n_threads = 8;
    const unsigned max_depth = 600;

    //reset current depth before search
    depth = 0;

    //start threads here
    std::thread workers[n_threads];

    for (unsigned i = 0; i < n_threads; i++) {
        workers[i] = std::thread(&agent::mcts, this, max_depth);
    }
    for (unsigned i = 0; i < n_threads; i++) {
        workers[i].join();
    }
}

move agent::act(const state& s, const move& m )
{
    //check if state is already in calculation
    bool reassign = !root->inherit(s);
    if (reassign) {
        root.reset(new node(s, std::vector<state>(), m));
        root->expand(pn);
    }

    dirichlet_noise();

    think();

    unsigned index = 0;
    int highscore = 0;
    for (unsigned i = 0; i < root->size(); i++) {
        int score = root->get(i)->n();
        if (highscore < score) {
            highscore = score;
            index = i;
        }
    }
   
    //make root the chosen child
    move action = root->get(index)->action();
    played_moves.push_back(action);
    root->inherit(index);

    return action;
}

void agent::train(float target)
{   
    //set both networks to training mode
    pn->train();
    vn->train();

    //stack the recorded positions into batch
    torch::Tensor inputs = torch::stack(positions);
    inputs.to(device);
    std::cout << inputs.sizes()[0] << std::endl;

    //valnet
    // 
    //generate target batch from score
    val_adam->zero_grad();

    torch::Tensor y_val = torch::ones({ (long long)positions.size(), 1 }, device);
    y_val *= target;
    for (unsigned i = 1; i < positions.size(); i += 2) {  //we have to switch the result for all the black turns
        y_val[i] *= -1;
    }

    //train using mean squared error
    torch::Tensor x_val = vn->forward(inputs);
    x_val.to(device);
    torch::Tensor loss_val = torch::mse_loss(x_val, y_val);
    loss_val.to(device);
    std::cout << "value loss: " << loss_val.mean() << std::endl;
    loss_val.backward();
    val_adam->step();

    //polnet
    //
    //generate target batch from played moves
    pol_adam->zero_grad();

    mdis finder;
    auto y_options = torch::TensorOptions().dtype(torch::kInt64).device(torch::kCUDA, 0);
    torch::Tensor y_pol = torch::empty({ (int)played_moves.size() }, y_options);
    for (int i = 0; i < played_moves.size(); i++) {
        if (!(i % 2))
            y_pol.index_put_({ i }, (long)finder.inverse_find(played_moves[i]));
        else
            y_pol.index_put_({ i }, (long)finder.find(played_moves[i]));
    }

    //generate predictions
    torch::Tensor x_pol = pn->forward(inputs);
    x_pol.to(device);

    //train using cross entropy loss
    torch::Tensor loss_pol = torch::cross_entropy_loss(x_pol, y_pol);
    loss_pol.backward();
    pol_adam->step();

    //reset everything
    played_moves.clear();
    positions.clear();

    pn->eval();
    vn->eval();
    
    //save the parameters
    torch::save(vn, "valnet.pt");
    torch::save(pn, "polnet.pt");
}

move agent::train_act(const state& s, std::vector<state> history, const move& m)
{
    //this function replaces "act" during training and involves making random moves. 
    //as it is used exclusively in self play, the reassignment in the end is deleted
    //the reassignment in the beginning is enforced to prevent memory overflow

    //check if state is already in calculation
    bool reassign = true; // !root->inherit(s);
    if (reassign) {
        root.reset(new node(s, history, m));
        root->expand(pn);
    }

    //the index of the move which will be chosen
    unsigned index = 0;

    //evaluate and append to predictions
    //double pos_eval = vn->forward(root->current()).item<double>();
    //std::cout << "estimated value of this position with " << root->color() << " to move is: " << pos_eval << std::endl;
    positions.push_back(position_convert(root->current()));

    //ensure exploration by using dirichlet distribution
    dirichlet_noise();

    think();

    //add extra exploration in first 15 moves by selecting moves at random (weighted by search)
    //choose best move if move is above move 15 (30 half moves)
    if (history.size() > 30) {
        int highscore = 0;
        for (unsigned i = 0; i < root->size(); i++) {
            int score = root->get(i)->n();
            if (highscore < score) {
                highscore = score;
                index = i;
            }
        }
    }
    else {
        int sum = 0;
        for (unsigned i = 0; i < root->size(); i++)
            sum += root->get(i)->n();

        std::random_device rnd;
        std::default_random_engine gen(rnd());
        std::uniform_int_distribution<> dist(0, sum - 1);
        int choice = dist(gen);

        for (unsigned i = 0; i < root->size(); i++) {
            int prob = root->get(i)->n();
            if (choice < prob) {
                index = i;
                break;
            }
            choice -= prob;
        }
    }

    //make root the chosen child
    move action = root->get(index)->action();
    played_moves.push_back(action);

    return action;
}

void agent::dirichlet_noise()
{
    const int size = root->size();

    std::random_device rnd;
    std::default_random_engine gen(rnd());
    std::gamma_distribution<float> gamma(0.3f, 1);

    float sum = 0;
    float* noise = new float[size];

    for (unsigned i = 0; i < size; i++) {
        noise[i] = gamma(gen);
        sum += noise[i];
    }
    for (unsigned i = 0; i < size; i++)
        root->set_move_prob((0.75f * root->move_prob()) + (0.25f * noise[i] / sum));

    delete[] noise;
}

double agent::UCB1(const node* child, int N)
{
    int cpuct_base = 19000;
    double cpuct = log((N + cpuct_base + 1) / cpuct_base) + c;
    double Q = (child->n()) ? (child->t() / (child->n() + child->o())) : 1.1;
    return Q + (cpuct * child->move_prob() * sqrt(N) / ((double)child->n() + child->o() + 1));
}

unsigned agent::select(node* parent)
{
	unsigned index = 0;
	float highscore = float(-INFINITY);
	for (unsigned i = 0; i < parent->size(); i++) {
	    double score = UCB1(parent->get(i), parent->n());
		if (highscore < score) {
			highscore = score;
			index = i;
		}
	}

	return index;
}

double agent::expand(node* Node)
{
	//for every legal move, we have to create a new node
	Node->expand(pn);
	return (Node->terminal()) ? eval(Node) : mcts_step(Node->get(select(Node)));
}

double agent::mcts_step(node* Node)
//expand the tree and its evaluation by one new evaluated node. returns eval of an examined child node
//To tell other threads in multithreaded MCTS that this node is already being evaluated, o has to be incremented 
//in the forward pass but decremented in the backprop
{
    Node->increment_o();
    double evaluation;

    //is this a terminal state?
    if (Node->terminal())
        evaluation = eval(Node);
    //no:
    else
    //is this a leaf node?
    if (!Node->expanded())
    {
        //has it been visited before?
        if (Node->n())
            //yes: expand and evaluate best child
        {
            evaluation = expand(Node); //check if terminal state and node cannot be expanded
        }
        else
            //no: evaluate this node
        {
            evaluation = eval(Node);
        }
    }
    else 
        //no (not a leaf node): pick the best child node to examine
    {
        evaluation = mcts_step(Node->get(select(Node)));
    }

    //backpropagate
    //the evaluation has to be flipped. a black node with white winning needs val -x, with black winning x and vice versa.
    Node->increment_t(-evaluation * Node->color()); 
    Node->increment_n();
    Node->decrement_o();
    return evaluation;
}

void agent::mcts(unsigned max_depth)
{
    while (depth < max_depth) {
        mcts_step(root.get());
    }
}

double agent::eval(const node* Node) //return a positive value if white is winning, a negative value if black is winning
{
    //an evaluation marks a full playout. increment depth here.
    dv.lock();
    depth++;
    dv.unlock();

    //return terminal state value if available
    //otherwise, predict and convert to double
    double returnval = (Node->terminal()) ? (double)Node->score() : 
        vn->forward(Node->current()).item<double>() * Node->color();

    //return neural net eval
    return returnval;
}

torch::Tensor agent::position_convert(const state& s)
{
    //set up the tensor from a given state
    torch::Tensor x = torch::zeros({ 6, 8, 8 }, device).contiguous();

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
                    x[ptype - 1][i][j] = pcolor;
            }
    else
        for (unsigned i = 0; i < 8; i++)
            for (unsigned j = 0; j < 8; j++) {
                piece p = s.position[i * 8 + j];
                unsigned ptype = p.get_type();
                int pcolor = p.get_color();
                if (pcolor)
                    x[ptype - 1][7 - i][7 - j] = -pcolor;
            }

    return x;
}