#include "chess_environment.h"
#include "agent.h"
#include "tools.h"
#include <thread>

void render(chess_environment& env)
{
    while (!glfwWindowShouldClose(env.get_window()))
        env.render();
}

void input(chess_environment& env, agent& agent)
{
    using namespace std::literals::chrono_literals;
    while (!glfwWindowShouldClose(env.get_window()))
    {
        env.player_input();
        std::this_thread::sleep_for(1s);
        env.agent_input(agent.act(env.get_state()));
    }
}

void play()
{
    using namespace std::literals::chrono_literals;

    chess_environment* env = new chess_environment;
    agent loris;
    int turn = WHITE;
    int color = WHITE;
    while (!glfwWindowShouldClose(env->get_window()))
    {
        static bool print = true;
        if (print && env->get_state().terminal_state) {
            switch (env->get_state().score) {
            case -1:
                std::cout << "black wins!";
                break;
            case 0:
                std::cout << "draw!";
                break;
            case 1:
                std::cout << "white wins!";
                break;
            default:
                std::cout << "damn, something really went wrong!";
            }
            print = false;
        }
        else {
            turn = env->get_state().turn;
            if (turn == color)
                env->player_input();
            else
                //env->agent_input(loris.act(env->get_state()));
                env->player_input();

        }
        env->render();
    }
}