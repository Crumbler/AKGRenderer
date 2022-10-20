#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"

class NormalTexture
{
    public:
        NormalTexture(const std::string& filename);
        glm::vec3 getNormal(float x, float y);

    private:
        std::vector<glm::vec3> normals;
        unsigned width, height;
};
