#pragma once

#include "GLM/glm/glm.hpp"
#include "GL/glew.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void use() const;

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setUniformMat4fv(const std::string& name, glm::mat4 value) const;
	void setUniform4f(const std::string& name, float value1, float value2, float value3, float value4 = 1.0f) const;
private:
	void checkShaderCompileError(const unsigned int& shader);
	void checkProgramLinkError(const unsigned int& program);
};

