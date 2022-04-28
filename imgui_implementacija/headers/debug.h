#pragma once
#include "glad/glad.h"

GLenum glCheckError_(const char* file, int line);

#define CHECK_OPENGL glCheckError_(__FILE__, __LINE__)