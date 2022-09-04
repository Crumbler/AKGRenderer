#pragma once

#include "GLRawModel.hpp"
#include <glad/glad.h>

class GLDisplayModel : public GLRawModel
{
    public:
        GLDisplayModel();
        GLDisplayModel(GLuint vaoID, GLuint vbo0);
        GLuint vbo0;
};
