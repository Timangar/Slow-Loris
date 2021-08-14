#pragma once
#include "GLM/glm/glm.hpp"
struct Hitbox
{
	Hitbox(float Diameter, glm::vec3 translation = glm::vec3(0.0f), int Scr_Width = 800, int Scr_Height = 600);
	Hitbox(float DiameterX, float DiameterY, glm::vec3 translation = glm::vec3(0.0f), int Scr_Width = 800, int Scr_Height = 600);
	void update(float DiameterX, float DiameterY, glm::vec3 translation, int Scr_Width, int Scr_Height);
	void getHitbox(float* x, float* y, float* origin);

	glm::vec4 origin;
	float TrueOriginX;
	float TrueOriginY;
	float x_left_boundry;
	float x_right_boundry;
	float y_upper_boundry;
	float y_lower_boundry;
};


