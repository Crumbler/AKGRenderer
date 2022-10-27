#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "TextureType.hpp"

class Texture
{
    public:
        Texture(const std::string& filename, TextureType type);
        glm::vec3 getCol(float x, float y);

    private:
        std::vector<unsigned char> data;
        unsigned width, height;
};
