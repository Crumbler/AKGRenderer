#include "Face.hpp"

Face::Face(const glm::ivec3 v, const glm::ivec3 t, const glm::ivec3 n)
{
    this->vertices = v;
    this->uvs = t;
    this->normals = n;
}
