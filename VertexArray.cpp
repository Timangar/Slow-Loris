#include "VertexArray.h"
#include "GLCall.h"

VertexArray::VertexArray()
{
	for (int i = 0; i < 20; i++)
		SizeRegister[i] = 0;
	LocCount = 0;
	Stride = 0;
	Normalize = 0;
	GLCall(glGenVertexArrays(1, &ID));
	GLCall(glBindVertexArray(ID));
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &ID);
}

void VertexArray::bind() const { GLCall(glBindVertexArray(ID)); }
void VertexArray::unbind() { GLCall(glBindVertexArray(0)); }

void VertexArray::attrib(int size)
{
	if (size == 0)
		std::cerr << "ERROR::VERTEXARRAY::ATTRIB::0 WAS PASSED" << std::endl;
	SizeRegister[LocCount] = size;
	Stride += size;
	LocCount++;
}

void VertexArray::generate()
{
	for (int i = 0; i < LocCount; i++)
	{
		int offset = 0;
		for (int j = 0; j < i; j++)
			offset += SizeRegister[j];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, SizeRegister[i], GL_FLOAT, Normalize, Stride * sizeof(float), (void*)(offset * sizeof(float))));
	}
}

void VertexArray::setNormalize(bool value)
{
	if (value == 0)
		Normalize = 0;
	else
		Normalize = 1;
}

/*
void VertexArray::operator=(VertexArray copy_vao)
{
	for (unsigned int i = 0; i < 20; i++)
		SizeRegister[i] = copy_vao.SizeRegister[i];
	Normalize = copy_vao.Normalize;
	Stride = copy_vao.Stride;
	LocCount = copy_vao.LocCount;
	generate();
	ID = copy_vao.ID;
}
*/