#include "Shader.h"

#include "GLM/glm/gtc/type_ptr.hpp"
#include "GLCall.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
//read shader code from file
	std::string VertexCode;
	std::string FragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//ensure error checking
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//read files into buffer
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close files
		vShaderFile.close();
		fShaderFile.close();
		//convert buffers into strings
		VertexCode = vShaderStream.str();
		FragmentCode = fShaderStream.str();
	}
	//evaluate possible errors
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER::FAILED TO READ FROM FILE" << std::endl;
		std::cerr << e.what() << std::endl;
	}
	//convert std::string into const char* for compatibility with glShaderSource
	const char* VertexShaderCode = VertexCode.c_str();
	const char* FragmentShaderCode = FragmentCode.c_str();

//compile shaders
	//Vertex Shader
	GLCall(unsigned int VertexShader = glCreateShader(GL_VERTEX_SHADER));
	GLCall(glShaderSource(VertexShader, 1, &VertexShaderCode, 0));
	GLCall(glCompileShader(VertexShader));
	checkShaderCompileError(VertexShader);
	//Fragment Shader
	GLCall(unsigned int FragmentShader = glCreateShader(GL_FRAGMENT_SHADER));
	GLCall(glShaderSource(FragmentShader, 1, &FragmentShaderCode, 0));
	GLCall(glCompileShader(FragmentShader));
	checkShaderCompileError(FragmentShader);

//link shaders into program & initialize ID class variable
	GLCall(ID = glCreateProgram());
	GLCall(glAttachShader(ID, VertexShader));
	GLCall(glAttachShader(ID, FragmentShader));
	GLCall(glLinkProgram(ID));
	checkProgramLinkError(ID);

//flag shaders for deletion
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
	std::cout << "Program deleted" << std::endl;
}

void Shader::use() const { glUseProgram(ID); }

void Shader::setBool(const std::string &name, bool value) const
{
	GLCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value));
}

void Shader::setInt(const std::string& name, int value) const
{
	GLCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), value));
}

void Shader::setFloat(const std::string& name, float value) const
{
	GLCall(glUniform1f(glGetUniformLocation(ID, name.c_str()), value));
}

void Shader::setUniformMat4fv(const std::string& name, glm::mat4 value) const
{
	unsigned int loc = glGetUniformLocation(ID, name.c_str());
	GLCall(glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]));
}

void Shader::setUniform4f(const std::string& name, float value1, float value2, float value3, float value4) const
{
	unsigned int loc = glGetUniformLocation(ID, name.c_str());
	GLCall(glUniform4f(loc, value1, value2, value3, value4));
}

void Shader::checkShaderCompileError(const unsigned int& shader)
{
	int success;
	char log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, log);
		std::cerr << "ERROR::SHADER::FAILED TO COMPILE SHADER\n" << log << std::endl;
	}
}

void Shader::checkProgramLinkError(const unsigned int& program)
{
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::FAILED TO LINK PROGRAM\n" << infoLog << std::endl;
	}
}