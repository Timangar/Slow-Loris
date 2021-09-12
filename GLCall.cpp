#include "GLCall.h"
#include "GL/glew.h"
#include <iostream>

void GLClearErr()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogErr(const char* function, const char* file, int line)
{
    bool err = false;
    while (GLenum Error = glGetError())
    {
        std::cerr << "ERROR: OPENGL ERRCODE (" << Error << ") IN FUNCTION (" << function << ") \n OF FILE ("
            << file << ") ON LINE (" << line << ")" << std::endl;
        err = true;
    }
    if (err)
        return false;
    else
        return true;
}