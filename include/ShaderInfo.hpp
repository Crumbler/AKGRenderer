#pragma once

#include <glad/glad.h>

struct ShaderInfo
{
    ShaderInfo(const char* file, GLenum type) : file(file), type(type) { };
    const char* file;
    GLenum type;
};
