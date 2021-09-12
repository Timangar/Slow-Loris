#include "Object.h"
#include <cassert>

int Object::ScreenWidth = 800;
int Object::ScreenHeight = 600;

Object::Object(const Shader* Program, VertexArray* VertexArray, Texture* Texture,  int VertexCount, const char* ModelLoc)
{
	shaderptr = Program;
	vaptr = VertexArray;
	textureptr = Texture;
	vertexCount = VertexCount;
	ModelTranslate = glm::vec3(0.0f);
	ModelRotate = glm::vec3(1.0f);
	ModelRotation = 0.0f;
	ModelScale = glm::vec3(0.0f);
	Model = glm::mat4(1.0f);
	GLCall(modelLoc = glGetUniformLocation(shaderptr->ID, ModelLoc));
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

Object::Object(const Shader* shader, int diameter, const char* texture, int VertexCount, const char* ModelLoc)
{
	unsigned int elements[] = { 0, 1, 2, 2, 3, 0 };
	shaderptr = shader;
	vaptr = new VertexArray();
	vaptr->bind();
	VertexBuffer vbo(diameter);
	assert(VertexCount == 6);
	IndexBuffer ebo;
	textureptr = new Texture(texture);
	textureptr->bind();
	vaptr->attrib(3);
	vaptr->attrib(2);
	vaptr->generate();

	vertexCount = VertexCount;
	ModelTranslate = glm::vec3(0.0f);
	ModelRotate = glm::vec3(1.0f);
	ModelRotation = 0.0f;
	ModelScale = glm::vec3(0.0f);
	Model = glm::mat4(1.0f);
	GLCall(modelLoc = glGetUniformLocation(shaderptr->ID, ModelLoc));
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

Object::~Object()
{
	delete vaptr;
	vaptr = nullptr;
	delete textureptr;
	textureptr = nullptr;
}

void Object::render()
{
	bindModelMat();
	textureptr->bind();
	vaptr->bind();
	GLCall(glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);)
}

void Object::scale(float Sx, float Sy, float Sz)
{
	ModelScale = glm::vec3(Sx, Sy, Sz);
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, ModelTranslate);
	Model = glm::rotate(Model, ModelRotation, ModelRotate);
	Model = glm::scale(Model, ModelScale);
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

void Object::scale(float S)
{
	ModelScale = glm::vec3(S, S, S);
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, ModelTranslate);
	Model = glm::rotate(Model, ModelRotation, ModelRotate);
	Model = glm::scale(Model, ModelScale);
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

void Object::rotate(float Degrees, float x, float y, float z)
{
	ModelRotate = glm::vec3(x, y, z);
	ModelRotation = Degrees;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, ModelTranslate);
	Model = glm::rotate(Model, glm::radians(ModelRotation), ModelRotate);
	Model = glm::scale(Model, ModelScale);
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

void Object::translate(double x, double y, double z)
{
	ModelTranslate = glm::vec3(x, y, z);
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, ModelTranslate);
	Model = glm::rotate(Model, ModelRotation, ModelRotate);
	Model = glm::scale(Model, ModelScale);
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

void Object::newModelMat(glm::mat4 modelMat)
{
	Model = modelMat;
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

void Object::bindModelMat()
{
	shaderptr->use();
	GLCall(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model)));
}

void Object::setScreenDimension(int width, int height)
{
	ScreenWidth = width;
	ScreenHeight = height;
}
