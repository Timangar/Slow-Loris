#pragma once

#include "GL/glew.h"
#include "GLCall.h"
#include "GLM/glm/gtc/matrix_transform.hpp"
#include "GLM/glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "GLM/glm/glm.hpp"

class Object
{
public:
	Object(const Shader* Program, VertexArray* VertexArray, Texture* Texture, int VertexCount, const char* ModelLoc = "model");
	Object(const Shader* shader, int diameter, const char* texture, int VertexCount = 6, const char* ModelLoc = "model");

	~Object();

	void render();

	virtual void scale(float Sx, float Sy, float Sz = 1.0f);
	virtual void scale(float S);
	virtual void rotate(float Degrees, float x, float y, float z);
	virtual void translate(double x, double y, double z);

	void newModelMat(glm::mat4 modelMat = glm::mat4(1.0f));
	void bindModelMat();

	void setScreenDimension(int width, int height);
protected:
	glm::vec3 ModelTranslate;
	glm::vec3 ModelRotate;
	glm::vec3 ModelScale;
	float ModelRotation;
	glm::mat4 Model;
	int modelLoc;
	static int ScreenWidth, ScreenHeight;
private:
	int vertexCount;
	const Shader* shaderptr;
	VertexArray* vaptr;
	Texture* textureptr;
};

