#include "Texture.h"
#include "GLCall.h"

#include "STBI/stb_image.h"

Texture::Texture(const char* Path) : Bound(true), BoundIn(GL_TEXTURE0)
{
	GLCall(glGenTextures(1, &ID));
	GLCall(glBindTexture(GL_TEXTURE_2D, ID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
	float bordercolor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor));

	stbi_set_flip_vertically_on_load(true);
	Data = stbi_load(Path, &width, &height, &nrChannels, 0);
	if (Data)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
		std::cerr << "ERROR::TEXTURE::FAILED TO CREATE TEXTURE FROM DATA" << std::endl;
	stbi_image_free(Data);
}

Texture::Texture() : Bound(true), BoundIn(GL_TEXTURE0)
{
	GLCall(glGenTextures(1, &ID));
	GLCall(glBindTexture(GL_TEXTURE_2D, ID));
	Data = 0;
	width = 0;
	height = 0;
	nrChannels = 0;
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::bind(unsigned int ActiveTexture)
{

	GLCall(glActiveTexture(GL_TEXTURE0 + ActiveTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, ID));
	Bound = true;
	BoundIn = GL_TEXTURE0 + ActiveTexture;
}

void Texture::unbind(unsigned int ActiveTexture)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + ActiveTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::setWrapParam(GLint param, float* BorderColor)
{
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param));
	if (param == GL_CLAMP_TO_BORDER)
		GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor));
}

void Texture::setFilterParam(GLint MagParam, GLint MinParam)
{
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagParam));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinParam));
}

void Texture::load(const char* Path, GLint Format)
{
	if (!Bound)
		bind();
	else
		GLCall(glActiveTexture(BoundIn));

	Data = stbi_load(Path, &width, &height, &nrChannels, 0);
	if (Data)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, Format, width, height, 0, Format, GL_UNSIGNED_BYTE, Data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
		std::cerr << "ERROR::TEXTURE::FAILED TO CREATE TEXTURE FROM DATA" << std::endl;
	stbi_image_free(Data);
}

float Texture::DefBorderColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };