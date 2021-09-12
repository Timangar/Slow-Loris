#pragma once
#include "GL/glew.h"

class VertexBuffer
{
public:
	unsigned int ID;

	VertexBuffer(const float* Data, int SizeOfData);
	VertexBuffer(int diameter);
	~VertexBuffer();

	void bind();
	void unbind();
};

