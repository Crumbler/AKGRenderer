#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Color
{
    static constexpr Color red() { return Color(255, 0, 0); }
    static constexpr Color green() { return Color(0, 255, 0); }
    static constexpr Color blue() { return Color(0, 0, 255); }
    static constexpr Color black() { return Color(0, 0, 0); }
    static constexpr Color white() { return Color(255, 255, 255); }
    static constexpr Color flat(const float f)
    {
        return Color(std::round(255.0f * f),
                     std::round(255.0f * f),
                     std::round(255.0f * f));
    }

    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : r(r), g(g), b(b) { }

    constexpr Color(uint8_t b = 0)
        : r(b), g(b), b(b) { }

    Color(const glm::vec3 v)
    {
        r = std::round(255.0f * v.x);
        g = std::round(255.0f * v.y);
        b = std::round(255.0f * v.z);
    }

    uint8_t r, g, b;
};
