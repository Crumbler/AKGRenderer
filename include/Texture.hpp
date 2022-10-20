#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"

#include "Color.hpp"

class Texture
{
    public:
        Texture(const std::string& filename);
        Color getCol(float x, float y);

    private:
        std::vector<unsigned char> data;
        unsigned width, height;
};
