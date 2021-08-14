#include "VertexBuffer.h"
#include "GLCall.h"

VertexBuffer::VertexBuffer(const float* Data, int SizeOfData)
{
	GLCall(glGenBuffers(1, &ID);)
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID);)
	GLCall(glBufferData(GL_ARRAY_BUFFER, SizeOfData, Data, GL_STATIC_DRAW);)
}

VertexBuffer::VertexBuffer(int diameter)
{
	float r = diameter / 2.0f;
	float Data[] = {
		//vertex Pos              texcoords
		 r,  r, 0.0f,             1.0f, 1.0f,         //top right
		-r,  r, 0.0f,             0.0f, 1.0f,         //top left
		-r, -r, 0.0f,             0.0f, 0.0f,         //bottom left
		 r, -r, 0.0f,             1.0f, 0.0f,         //bottom right
	};

	GLCall(glGenBuffers(1, &ID);)
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID);)
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Data), Data, GL_STATIC_DRAW);)
}

VertexBuffer::~VertexBuffer()
{
	//glDeleteBuffers(1, &ID);
}

void VertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VertexBuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}