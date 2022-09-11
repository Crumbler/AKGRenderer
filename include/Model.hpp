#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Model
{
    public:
        Model(const std::string& filename);

        std::vector<glm::vec4> vertices;
        std::vector<glm::ivec3> polygons;
};
