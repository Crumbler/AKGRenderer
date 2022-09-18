#pragma once

#include <cstdint>

struct Color
{
    static constexpr Color red() { return Color(255, 0, 0); }
    static constexpr Color green() { return Color(0, 255, 0); }
    static constexpr Color blue() { return Color(0, 0, 255); }
    static constexpr Color black() { return Color(0, 0, 0); }
    static constexpr Color white() { return Color(255, 255, 255); }

    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : r(r), g(g), b(b) { }

    constexpr Color(uint8_t b = 0)
        : r(b), g(b), b(b) { }

    uint8_t r, g, b;
};
