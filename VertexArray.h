#pragma once

#include "GL/glew.h"
#include <iostream>

class VertexArray
{
public:
	unsigned int ID;

	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind();

	void attrib(int size);
	void generate();

	void setNormalize(bool value);

	//void operator=(VertexArray copy_vao);

private:
	int LocCount;
	int Stride;
	bool Normalize;
	int SizeRegister[20];
};

