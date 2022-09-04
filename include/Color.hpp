#pragma once

#include <cstdint>

struct Color
{
    static constexpr Color red() { return Color(255, 0, 0); }
    static constexpr Color green() { return Color(0, 255, 0); }
    static constexpr Color blue() { return Color(0, 0, 255); }
    static constexpr Color black() { return Color(0, 0, 0); }
    static constexpr Color white() { return Color(255, 255, 255); }

    constexpr Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0)
        : r(r), g(g), b(b) { }

    uint8_t r, g, b;
};
