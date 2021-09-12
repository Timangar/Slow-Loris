#include "IndexBuffer.h"
#include "GLCall.h"

IndexBuffer::IndexBuffer(const unsigned int* Data, int SizeOfData)
{
	GLCall(glGenBuffers(1, &ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, SizeOfData, Data, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer()
{
	unsigned int Data[6] = { 0, 1, 2, 2, 3, 0 };
	GLCall(glGenBuffers(1, &ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Data), Data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	//GLCall(glDeleteBuffers(1, &ID));
}

void IndexBuffer::bind()
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
}

void IndexBuffer::unbind()
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
