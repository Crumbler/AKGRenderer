#pragma once

#include "DisplayShader.hpp"
#include "GLLoader.hpp"
#include <glad/glad.h>

class GLRenderer
{
    public:
        static void Prepare();
        static void Render();
        static void Init(int width, int height);
        static void UpdateDisplay(int width, int height, const void *data, bool sizeChanged);
        static void CleanUp();

    private:
        static DisplayShader shader;
        static GLLoader loader;
        static GLDisplayModel model;
        static GLuint texId;
};
