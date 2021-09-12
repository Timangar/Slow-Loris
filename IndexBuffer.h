#pragma once
#include "GL/glew.h"

class IndexBuffer
{
public:
	unsigned int ID;

	IndexBuffer(const unsigned int* Data, int SizeOfData);
	IndexBuffer();
	~IndexBuffer();

	void bind();
	void unbind();
};

