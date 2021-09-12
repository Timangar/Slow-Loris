#include "chess_environment.h"
#include "agent.h"
#include "tools.h"
#include <thread>

static bool render = false;

int perf(chess_environment &env, int depth)
{
    using namespace std::literals::chrono_literals;
    int n = 0;
    if (env.get_state().terminal_state || !depth)
        return 1;
    //std::this_thread::sleep_for(0.01s);
    for (const move& m : env.get_state().legal_moves)
    {
        env.agent_input(m);
        if (render)
            env.render();
        n += perf(env, depth -1);
        env.unmake_move();
    }
    return n;
}

void perft()
{
    chess_environment env(chess_environment::start_fen, render);
    for (int i = 1; i < 10; i++) {
        auto start = std::chrono::system_clock::now();
        int variation_count = perf(env, i);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "depth = " << i << "; time = " << diff.count() << "; variation count = " << variation_count << std::endl;
    }
    
    std::cout << "end of test. have fun debugging!";
}