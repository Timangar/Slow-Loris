#pragma once
#include "state.h"
#include "graphics_engine.h"
#include "legal_move_generator.h"

class chess_environment
{
public:
	//public class functions:
	chess_environment(std::string fen = start_fen, bool user_interface = true);
	~chess_environment();

	void reset();
	void set(const state& state);
	void set(std::string fen);
	void unmake_move();

	std::vector<state>& get_history();
	state& get_state(); //contains board position, repetition count, color of next turn, legal moves, termination state and static evaluation
	GLFWwindow* get_window(); 

	void render(); //renders the environment

	void player_input();
	void agent_input(move action);

	static std::string const start_fen;
private:
	//graphics stuff
	void get_mouse(GLFWwindow* window, double* xpos, double* ypos, bool* lclick);
	bool insquare(double mx, double my, unsigned int square);

	int grabbed_piece;

	double mx, my;	//cursor x and y position
	bool click;     //left mouse button

	graphics_engine* engine;

	//backend stuff
	void step(move action); //takes in a move and steps the environment accordingly

	lmg gen;
	std::vector<state> history;
	state current_state;

};

