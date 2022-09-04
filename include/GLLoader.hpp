#pragma once

#include <vector>
#include <glad/glad.h>
#include "GLDisplayModel.hpp"

class GLLoader
{
    public:
        GLDisplayModel loadDisplayModel(const std::vector<float>& positions);
        GLuint createTexture(int width, int height);
        void updateTexture(GLuint texId, int width, int height, const void *data, bool sizeChanged);

        void CleanUp();

    private:
        std::vector<GLuint> vaos, vbos, textures;

        int createVAO();
        GLuint storeStatic(int attributeNumber, const std::vector<float>& data);
        void unbindVAO();
};
