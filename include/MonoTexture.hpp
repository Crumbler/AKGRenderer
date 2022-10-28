#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "TextureType.hpp"

class MonoTexture
{
    public:
        MonoTexture(const std::string& filename, TextureType type);
        float getVal(float x, float y);

    private:
        std::vector<unsigned char> data;
        unsigned width, height;
};
