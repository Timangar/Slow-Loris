#include "Board.h"
#include "DEFINES.h"

Board::Board(const Shader* Shader, const char* Texture, int VertexCount, float Diameter, const char* ModelLoc)
	: Object(Shader, Diameter, Texture, VertexCount, ModelLoc), initialD(Diameter)
{
	diameter = Diameter;
	squares_init(squares);
}

Board::~Board()
{
}

void Board::scale(float Sx, float Sy, float Sz)
{
	ModelScale = glm::vec3(Sx, Sy, Sz);
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, ModelTranslate);
	Model = glm::rotate(Model, ModelRotation, ModelRotate);
	Model = glm::scale(Model, ModelScale);
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
	diameter = initialD * Sx;
	squares_init(squares);
}

void Board::translate(float x, float y, float z)
{
	ModelTranslate = glm::vec3(x, y, z);
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, ModelTranslate);
	Model = glm::rotate(Model, ModelRotation, ModelRotate);
	Model = glm::scale(Model, ModelScale);
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
	squares_init(squares);
}

glm::vec2 Board::get_square(int index)
{
	assert(index > -1 && index < 64);
	return squares[index];
}

float Board::get_diameter()
{
	return diameter;
}

void Board::squares_init(glm::vec2* a)
{
	const double sd = diameter / 8; //square diameter
	for (int y = 0; y < 8; y++)
	{
		double y_coord = ModelTranslate.y + 0.5 * diameter - sd * (y + .5f);
		for (int x = 0; x < 8; x++)
		{
			double x_coord = ModelTranslate.x - 0.5 * diameter + sd * (x + .5f);
			a[x + 8 * y] = glm::vec2(x_coord, y_coord);
		}
	}
}
