#include "Hitbox.h"

Hitbox::Hitbox(float Diameter, glm::vec3 translation, int Scr_Width, int Scr_Height)
{
	update(Diameter, Diameter, translation, Scr_Width, Scr_Height);
}

Hitbox::Hitbox(float DiameterX, float DiameterY, glm::vec3 translation, int Scr_Width, int Scr_Height)
{
	update(DiameterX, DiameterY, translation, Scr_Width, Scr_Height);
}

void Hitbox::update(float DiameterX, float DiameterY, glm::vec3 translation, int Scr_Width, int Scr_Height)
{
	origin = { 0.0f, 0.0f, 0.0f, 1.0f };
	origin = { translation, 1.0f };
	TrueOriginX = origin.x + Scr_Width / 2;
	TrueOriginY = -origin.y + Scr_Height / 2;

	x_left_boundry = TrueOriginX - DiameterX / 2;
	x_right_boundry = TrueOriginX + DiameterX / 2;
	y_upper_boundry = TrueOriginY - DiameterY / 2;
	y_lower_boundry = TrueOriginY + DiameterY / 2;
}

void Hitbox::getHitbox(float* x_boundry, float* y_boundry, float* origin)
{
	x_boundry[0] = x_left_boundry;
	x_boundry[1] = x_right_boundry;
	y_boundry[0] = y_upper_boundry;
	y_boundry[1] = y_lower_boundry;

	origin[0] = TrueOriginX;
	origin[1] = TrueOriginY;
}
