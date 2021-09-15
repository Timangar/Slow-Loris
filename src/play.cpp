#include "chess_environment.h"
#include "agent.h"
#include "tools.h"
#include <chrono>
#include "play.h"

void input(chess_environment* env, agent& loris)
{
    static int turn = WHITE;
    static int color = WHITE;
 auto begin = std::chrono::high_resolution_clock::now();

    turn = env->get_state().turn;

    if (turn == color)
        env->player_input();

    else {
        auto begin = std::chrono::high_resolution_clock::now();
        while (true) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - begin;
            if (3 <= duration.count()) {
                env->agent_input(loris.act());
                break;
            }
            else
                env->render();
        }
    }
}

void play()
{
    using namespace std::literals::chrono_literals;

    chess_environment* env = new chess_environment;
    agent loris;
    loris.think_about(env->get_state());
    while (!glfwWindowShouldClose(env->get_window()))
    {
        static bool print = true;
        if (env->get_state().terminal_state) {
            if (print) {
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
        }
        else
            input(env, loris);
        env->render();
    }
}