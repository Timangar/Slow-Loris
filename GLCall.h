#pragma once

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearErr(); x; ASSERT(GLLogErr(#x, __FILE__, __LINE__));

void GLClearErr();

bool GLLogErr(const char* function, const char* file, int line);
