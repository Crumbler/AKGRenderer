#pragma once

#include <glm/glm.hpp>

struct Face
{
    Face(const glm::ivec3 v, const glm::ivec3 t, const glm::ivec3 n);

    glm::ivec3 vertices, uvs, normals;
};
