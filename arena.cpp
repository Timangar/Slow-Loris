#include "chess_environment.h"
#include <random>
#include "agent.h"
#include <math.h>

void arena(std::string engine1, std::string engine2, int epochs, int render_interval)
{
	int white_w = 0;
	int black_w = 0;
	int draw = 0;

	chess_environment* env = new chess_environment;
	agent* player1 = new agent(true, 2.2, 0, engine1);
	agent* player2 = new agent(true, 2.2, 0, engine2);

	for (unsigned epoch = 0; epoch < epochs; epoch++)
	{
		//env->set("3qk3/8/8/8/8/8/8/4K3 w - - 0 1");
		env->reset();
		unsigned i = 0;
		auto begin = std::chrono::high_resolution_clock::now();

		while (!env->get_state().terminal_state)
		{
			if (!(i % 2))
				env->agent_input(player1->act(env->get_state()));
			else
				env->agent_input(player2->act(env->get_state()));

			if (epoch % render_interval == 0) {
				env->render();
			}
			i++;
		}
		float score = env->get_state().score;

		if (score > 0)
			white_w++;
		else if (score < 0)
			black_w++;
		else
			draw++;

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds> (end - begin);

		std::cout << "epoch: " << epoch << std::endl
			<< "winner: " << score << std::endl
			<< "elapsed time: " << duration.count() << std::endl
			<< "score: " << white_w << "//" << draw << "//" << black_w << std::endl << std::endl;
	}
}