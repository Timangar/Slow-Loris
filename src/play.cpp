#include "chess_environment.h"
#include "tools.h"

int main(void)
{
    state s("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    chess_environment env;
    while (!glfwWindowShouldClose(env.get_window()))
    {
        env.player_input();
        env.render();
    }
}