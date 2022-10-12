#pragma once

#include <glm/glm.hpp>
#include "Color.hpp"

struct Vertex
{
    glm::vec3 v, n, posView;

    static Vertex Combine(const Vertex a, const Vertex b, const float ratio);
};
