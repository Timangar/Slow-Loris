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
    const unsigned n_threads = 4;
    const unsigned max_depth = 400;

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
        root.reset(new node(s, m));
        root->expand(pn);
    }

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
    //valnet
    // 
    //stack the recorded predictions into batch
    torch::Tensor x = torch::stack(predictions);
    x.to(device);
    x = x.view(predictions.size());

    //generate target batch from score
    torch::Tensor y = torch::ones({ (long long)predictions.size() }, device);
    y *= target;
    for (unsigned i = 1; i < predictions.size(); i += 2) {  //we have to switch the result for all the black turns
        y[i] *= -1;
    }

    //train using mean squared error
    torch::Tensor loss = torch::mse_loss(x, y);
    loss.to(device);
    loss.backward();
    val_adam->step();

    //polnet
    //
    //stack the recorded predictions into batch
    x = torch::stack(polnet_training_data);
    
    //generate target batch from played moves
    mdis finder;
    auto y_options = torch::TensorOptions().dtype(torch::kInt64).device(torch::kCUDA, 0);
    y = torch::empty({ (int)played_moves.size() }, y_options);
    for (int i = 0; i < played_moves.size(); i++) {
        y.index_put_({ i }, (long)finder.find(played_moves[i]));
    }

    x.to(device);

    torch::Tensor loss2 = torch::zeros({ 0 }, device);
    loss2 = torch::cross_entropy_loss(x, y);

    loss2.backward();
    pol_adam->step();

    //reset everything
    val_adam->zero_grad();
    pol_adam->zero_grad();
    predictions.clear();
    played_moves.clear();
    polnet_training_data.clear();
    
    //save the parameters
    torch::save(vn, "valnet.pt");
    torch::save(pn, "polnet.pt");
}

move agent::train_act(const state& s, float epsilon, const move& m)
{
    //this function replaces "act" during training and involves making random moves. 
    //as it is used exclusively in self play, the reassignment in the end is deleted

    //check if state is already in calculation
    bool reassign = true;//!root->inherit(s);
    if (reassign) {
        root.reset(new node(s, m));
        root->expand(pn);
    }

    //the index of the move which will be chosen
    unsigned index = 0;

    //evaluate and append to predictions
    //double pos_eval = vn->forward(root->current()).item<double>();
    //std::cout << "estimated value of this position with " << root->color() << " to move is: " << pos_eval << std::endl;
    predictions.push_back(vn->forward(root->current()));
    polnet_training_data.push_back(pn->forward(root->current(), 1));

    //ensure exploration by using dirichlet distribution
    dirichlet_noise();

    think();

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
        noise[i] = root->get(i)->move_prob();
        sum += noise[i];
    }
    for (unsigned i = 0; i < size; i++)
        root->set_move_prob((0.75f * root->move_prob()) + (0.25f * noise[i] / sum));

    delete[] noise;
}

double agent::UCB1(const node* child, int N)
{
	if (child->n())
		return child->t() / ((double)child->n() + (double)child->o()) + c * child->move_prob() * sqrt(log(N) / ((double)child->n() + (double)child->o()));
	else
		return c * child->move_prob() / (1 + (double)child->o());
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
    //return terminal state value if available
    //otherwise, predict and convert to double
    double returnval = (Node->terminal()) ? (double)Node->score() : 
        vn->forward(Node->current()).item<double>() * Node->color();

    //an evaluation marks a full playout. increment depth here.
    dv.lock();
    depth++;
    dv.unlock();

    //return neural net eval
    return returnval;
}

/*
//is this a leaf node?
    if (!Node.children().size())
        //yes: has it been visited before ?
    {
        if (Node.n())
            //yes: expand and evaluate best child (or
        {
            if (Node.terminal())                //check if terminal state and node cannot be expanded
                return Node.score();
            else
                evaluation = expand(Node);
        }
        else
            //no: is another thread currently running on this node?
        {
            if (workers_as_entering >= 2)
                //yes: expand and evaluate best child
            {
                if (Node.terminal())                //check if terminal state and node cannot be expanded
                    return Node.score();
                evaluation = expand(Node);
            }
            else
                //no: evaluate this node
            {
                evaluation = eval(Node);
            }
        }
    }
    else
        //no: pick the best child node to examine
    {
        evaluation = mcts_step(Node.children()[select(Node)]);
    }

    //backpropagate
    //the evaluation has to be flipped. a black node with white winning needs val -x, with black winning x and vice versa.
    Node.increment_t(-evaluation * Node.color());
    Node.increment_n();
    Node.decrement_o();
    return evaluation;
*/