#pragma once
#define GLEW_STATIC

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "iostream"
#include "state.h"

#include "Button.h"
#include "Board.h"
#include "GLCall.h"
#include "Object.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

class graphics_engine
{
public:
	graphics_engine();
	~graphics_engine();
	void render(const state& s, move m);
	void render(const state& s, int grabbed_piece, double mx, double my);

	GLFWwindow* get_window();

	int get_scr_w();
	int get_scr_h();
	Board* get_board();
protected:
	Object* white_pieces_init(int scaler);
	Object* black_pieces_init(int scaler);

	Shader* shader;
	GLFWwindow* window;
	GLFWmonitor* monitor;
	const GLFWvidmode* mode;
	int scr_w, scr_h;

	Board* board;
	Object* white_pieces;
	Object* black_pieces;
};

