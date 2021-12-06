#include "chess_environment.h"
#include "tools.h"
std::string const chess_environment::start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

chess_environment::chess_environment(std::string fen, bool user_interface) : current_state(fen) 
{
    if (user_interface)
        engine = new graphics_engine;
    else {
        std::cout << "WARNING! USER INTERFACE DISABLED, DO NOT RENDER!" << std::endl;
        engine = nullptr;
    }
    gen.gen(current_state, { 0,0 }, history, true);
    grabbed_piece = 0;
    mx = 0.0;
    my = 0.0;
    click = 0;
}

chess_environment::~chess_environment()
{
    delete engine;
}

void chess_environment::step(move m)
{
    if (current_state.contains(m) && !current_state.terminal_state) {
        history.push_back(current_state);
        current_state.turn *= -1;
        gen.gen(current_state, m, history);
    }
    else
        return;
}

void chess_environment::reset()
{
	history.clear();
	current_state = state(start_fen);
    gen.gen(current_state, { 0,0 }, history, true);
}

void chess_environment::set(const state& state) //TODO: test the copy constructor
{
	reset();
	current_state = state;
    gen.gen(current_state, { 0,0 }, history, true);
}

void chess_environment::set(std::string fen)
{
	reset();
	current_state = state(fen);
    gen.gen(current_state, { 0,0 }, history, true);
}

void chess_environment::unmake_move()
{
    if (history.size()) {
        current_state = history.at(history.size() - 1);
        history.pop_back();
    }
}

std::vector<state>& chess_environment::get_history()
{
    return history;
    std::vector<state> hist;
    hist.reserve(current_state.fifty_move_count);
    if (!history.size())
        return hist;
    for (int i = current_state.fifty_move_count; i > 0; i--)
        hist.push_back(history.at(history.size() - i));
    //std::cerr << current_state.fifty_move_count << " " << history.size();
    return hist;
}

state& chess_environment::get_state()
{
	// TODO: insert return statement here
	return current_state;
}

GLFWwindow* chess_environment::get_window()
{
    return engine->get_window();
}

void chess_environment::render()
{
    engine->render(current_state, grabbed_piece, mx, my);
}

void chess_environment::player_input()
{
    //handle input
    //get the mouse input
    get_mouse(engine->get_window(), &mx, &my, &click);

    //determine if any piece has been grabbed if none has been grabbed already
    if (!grabbed_piece) {

        for (int i = 0; i < 64; i++)
            if (insquare(mx, my, i) && click 
                && current_state.position[i].get_color() == current_state.turn) {
                grabbed_piece = i + 1;
                break;
            }
    }
    else {
        if (!click)
            for (int i = 0; i < 64; i++)
                if (insquare(mx, my, i)) {
                    if (grabbed_piece - 1 != i)
                        step({ grabbed_piece - 1, i });
                    grabbed_piece = 0;
                    break;
                }
    }
}

void chess_environment::agent_input(move action)
{
    step(action);
}

void chess_environment::get_mouse(GLFWwindow* window, double* xpos, double* ypos, bool* lclick)
{
    glfwGetCursorPos(window, xpos, ypos);
    *xpos = *xpos - engine->get_scr_w() / 2;
    *ypos = engine->get_scr_h() / 2 - *ypos;
    *lclick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
}

bool chess_environment::insquare(double mx, double my, unsigned int square)
{
    double sd = engine->get_board()->get_diameter() / 16; //the square "radius"
    double r = engine->get_board()->get_square(square).x + sd; //right boundry of square
    double l = engine->get_board()->get_square(square).x - sd; //left boundry of square
    double t = engine->get_board()->get_square(square).y + sd; //top boundry of square
    double b = engine->get_board()->get_square(square).y - sd; //bottom boundry of square
    if (l < mx && mx < r && b < my && my < t)
        return true;
    else
        return false;
}
