#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"

class Texture
{
    public:
        Texture(const std::string& filename);
        glm::vec3 getCol(float x, float y);

    private:
        std::vector<unsigned char> data;
        unsigned width, height;
};
