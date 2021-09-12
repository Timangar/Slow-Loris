#pragma once
#include "Hitbox.h"
#include "Object.h"
class Button : public Object
{
public:
	Button(Shader* Shader, VertexArray* Vao, Texture* Texture, float DiameterX, float DiameterY, glm::vec3 translation,
		int Scr_Width, int Scr_Height);
	bool GetStatus(double cursorPosX, double cursorPosY, bool MouseClick);
private:
	Hitbox HitBox;
};

