#include "Utils.hpp"

#include <windows.h>

float Utils::perpDotProduct(const glm::vec2 a, const glm::vec2 b)
{
    return a.x * b.y - a.y * b.x;
}
