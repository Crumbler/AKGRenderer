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

    static constexpr Color mul(const Color c, const float f)
    {
        return Color(std::round((float)c.r * f),
                     std::round((float)c.g * f),
                     std::round((float)c.b * f));
    }

    static constexpr Color combine(const Color a, const Color b)
    {
        const int nr = std::min(255, (int)a.r + (int)b.r),
            ng = std::min(255, (int)a.g + (int)b.g),
            nb = std::min(255, (int)a.b + (int)b.b);

        return Color(nr, ng, nb);
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
