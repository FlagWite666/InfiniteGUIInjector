#pragma once
#include "gl\glew.h"

GLuint LoadTextureFromMemory(const unsigned char* buffer, size_t size, int* outWidth = nullptr, int* outHeight = nullptr);