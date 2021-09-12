#include "Button.h"

Button::Button(Shader* Shader, VertexArray* Vao, Texture* Texture, float DiameterX, float DiameterY, glm::vec3 translation,
	int Scr_Width, int Scr_Height)
	: Object(Shader, Vao, Texture, 6), HitBox(DiameterX, DiameterY, translation, Scr_Width, Scr_Height)
{
	translate(translation.x, translation.y, translation.z);
}

bool Button::GetStatus(double cursorPosX, double cursorPosY, bool MouseClick) //enter in mouse coordinates!!!
{
	float x_boundry[2];
	float y_boundry[2];
	float origin[2];
	HitBox.getHitbox(x_boundry, y_boundry, origin);
	bool InX = x_boundry[0] < cursorPosX && cursorPosX < x_boundry[1];
	bool InY = y_boundry[0] < cursorPosY && cursorPosY < y_boundry[1];
	if (InX && InY && MouseClick)
		return 1;
	else
		return 0;
}

