#pragma once
#include "Object.h"

class Board : public Object
{
public:
	Board(const Shader* Shader, const char* Texture,
		int VertexCount = 6, float Diameter = 200.0f, const char* ModelLoc = "model");
	~Board();

	virtual void scale(float Sx = 1.0f, float Sy = 1.0f, float Sz = 1.0f);
	virtual void translate(float x, float y, float z);

	glm::vec2 get_square(int index);
	float get_diameter();

private:
	void squares_init(glm::vec2* s);
	const float initialD;
	float diameter;
	glm::vec2 squares[64];
};

