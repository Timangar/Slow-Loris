#include "chess_environment.h"
#include <random>
#include "agent.h"
#include <math.h>

void train(int epochs, int render_intervall, float epsilon, float decay)
{
	chess_environment* env = new chess_environment;
	agent* loris = new agent(true, 2.5, 0.00001);

	for (unsigned epoch = 0; epoch < epochs; epoch++)
	{
		//env->set("3qk3/8/8/8/8/8/8/4K3 w - - 0 1");
		env->reset();
		epsilon *= decay;
		unsigned i = 0;
		auto begin = std::chrono::high_resolution_clock::now();

		while (!env->get_state().terminal_state)
		{
			env->agent_input(loris->train_act(env->get_state(), env->get_history()));
			if (epoch % render_intervall == 0) {
				env->render();
			}
			i++;
		}
		float score = env->get_state().score;

		//calculate target if score is draw - we must "cheat" to speed up training
		if (!score) {
			//define piece values
			float values[7] = { 0.0f, 0.0f, 0.9f, 0.3f, 0.3f, 0.5f, 0.1f };

			for (piece p : env->get_state().position)
				score += values[p.get_type()] * p.get_color();

			score = tanh(score);
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds> (end - begin);

		std::cout << "epoch: " << epoch << std::endl
			<< "winner: " << score << std::endl
			<< "elapsed time: " << duration.count() << std::endl << std::endl;

		loris->train(score);
	}
}