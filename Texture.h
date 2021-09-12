#pragma once

#include "GL/glew.h"
#include <iostream>

class Texture
{
public:
	unsigned int ID;

	Texture(const char* Path);
	Texture();
	~Texture();

	void bind(unsigned int ActiveTexture = 0);
	static void unbind(unsigned int ActiveTexture);

	void setWrapParam(GLint param, float* BorderColor = DefBorderColor);
	void setFilterParam(GLint MagParam, GLint MinParam);

	void load(const char* Path, GLint Format = GL_RGB);

private:
	unsigned char* Data;
	int width, height, nrChannels;
	static float DefBorderColor[4];
	bool Bound;
	int BoundIn;
};

